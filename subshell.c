#include <stdio.h>
#include <util.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "subshell.h"


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
    free(subshell);
}

void send_command(Subshell *subshell, const char *cmd) {
    write(subshell->master_fd, cmd, strlen(cmd));
    
    // Skip echoed command
    char c;
    do {
        read(subshell->master_fd, &c, 1);
    } while (c != '\n');

    flush_output(subshell->master_fd);
}

void flush_output(int fd) {
    char buf[1024];
    int n;
    while ((n = read(fd, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("%s", buf);
        if (strstr(buf, "$ ")) {
            break;
        }
    }
}
