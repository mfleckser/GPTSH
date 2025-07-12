#include <sys/types.h>

#define MAX_HISTORY_LEN 50

typedef struct history {
    struct history *next;
    char *input;
    char *output;
} History;

typedef struct subshell {
    History *history;
    int history_len;
    pid_t pid;
    int master_fd;
} Subshell;


Subshell *init_subshell(const char *);
void free_subshell(Subshell *);
void send_command(Subshell *, const char *, int);
void flush_output(int, History *);
void print_history(History *);
