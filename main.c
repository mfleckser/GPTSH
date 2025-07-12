#include <stdio.h>
#include <stdlib.h>

#include "subshell.h"
#include "utils.h"

#define BUF_SIZE 1024


char SYSTEM_PROMPT[] = "You are a helpful and expert AI assistant embedded within a \
terminal running zsh on MacOS. A history of up to 50 previous user commands and \
their outputs are provided along with the user's query. Response to the user's \
question as concisely and accurately as possible, do not add extra fluff to your \
response. If you are missing context, you may provide the user a command to run \
and the output will be present with their next query, to which you can answer more \
accurately using the updated context. For formatting purposes, surround any command \
that you want the user to run with <CMD></CMD>.";

int main() {
    char buf[BUF_SIZE];

    Subshell *subshell = init_subshell("/bin/zsh");
    printf("Subshell started with pid: %d\n", subshell->pid);
    send_command(subshell, "stty -echo\n");
    flush_output(subshell->master_fd);

    while (fgets(buf, BUF_SIZE, stdin) != NULL) {
        if (buf[0] == ':') {
            send_prompt(buf + 1, SYSTEM_PROMPT);
        } else {
            send_command(subshell, buf);
        }
    }

    free_subshell(subshell);

    return 0;
}
