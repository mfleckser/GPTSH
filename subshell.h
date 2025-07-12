#include <sys/types.h>

typedef struct subshell {
    pid_t pid;
    int master_fd;
} Subshell;


Subshell *init_subshell(const char *);
void free_subshell(Subshell *);
void send_command(Subshell *, const char *);
void flush_output(int);
