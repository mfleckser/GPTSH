#include <stdio.h>
#include <util.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "subshell.h"


void free_history(History *history) {
    if (history) {
        free(history->input);
        free(history->output);
        free_history(history->next);
        free(history);
    }
}

History *add_history(Subshell *subshell) {
    History *new_history = (History *) malloc(sizeof(History));

    if (!subshell->history) {
        // First history entry
        subshell->history = new_history;
    } else {
        // Traverse history list and add history to tail
        History *prev_history = subshell->history;
        while (prev_history->next) {
            prev_history = prev_history->next;
        }
        prev_history->next = new_history;
    }

    if (subshell->history_len >= MAX_HISTORY_LEN) {
        History *to_free = subshell->history;
        subshell->history = subshell->history->next;
        to_free->next = NULL;
        free_history(to_free);
    } else {
        subshell->history_len++;
    }

    return new_history;
}

void print_history(History *history) {
    printf("INPUT:\n%sOUTPUT:\n%s", history->input, history->output);
    if (history->next) {
        print_history(history->next);
    }
}

/* Init shell subprocess using the given shell and return Subshell
 * struct with pid, etc.
*/
Subshell *init_subshell(const char *shellpath) {
    int master_fd;
    pid_t pid = forkpty(&master_fd, NULL, NULL, NULL);
    
    if (pid < 0) {
        fprintf(stderr, "Subshell initialization failed: fork unsuccussful.\n");
        exit(1);
    }


    if (pid) { // Parent
        // Initialize subshell
        Subshell *subshell = (Subshell *) malloc(sizeof(Subshell));
        subshell->pid = pid;
        subshell->master_fd = master_fd;
        subshell->history_len = 0;

        return subshell;
    } else {
        execl(shellpath, shellpath, NULL);
        perror("execl");
        exit(1);
    }
}

void free_subshell(Subshell *subshell) {
    write(subshell->master_fd, "exit\n", 5);
    close(subshell->master_fd);
    waitpid(subshell->pid, NULL, 0);
    free_history(subshell->history);
    free(subshell);
}

void send_command(Subshell *subshell, const char *cmd, int save_history) {
    write(subshell->master_fd, cmd, strlen(cmd));

    // Add input to subshell history
    History *cmd_info = NULL;
    if (save_history) {
        cmd_info = add_history(subshell);
        cmd_info->input = (char *) malloc(strlen(cmd) + 1);
        strcpy(cmd_info->input, cmd);
    }

    // Skip echoed command
    char c;
    do {
        read(subshell->master_fd, &c, 1);
    } while (c != '\n');

    flush_output(subshell->master_fd, cmd_info);
}

void flush_output(int fd, History *cmd_info) {
    char buf[1024];
    int n, total_len = 0;
    while ((n = read(fd, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("%s", buf);
        if (strstr(buf, "$ ")) {
            break;
        }

        total_len += n;
        if (cmd_info) {
            if (!cmd_info->output) {
                cmd_info->output = (char *) malloc(n + 1);
            } else {
                cmd_info->output = (char *) realloc(cmd_info->output, total_len + 1);
            }
            strcpy(cmd_info->output + (total_len - n), buf);
        }
    }
}
