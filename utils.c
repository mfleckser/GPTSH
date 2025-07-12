#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "cjson/cJSON.h"

#include "utils.h"


int parse_json_response(char *buf, const char *res) {
    cJSON *root = cJSON_Parse(res);
    if (!root) {
        // fprintf(stderr, "Failed to parse JSON response.\n");
        return -1;
    }

    cJSON *candidates = cJSON_GetObjectItemCaseSensitive(root, "candidates");
    if (!cJSON_IsArray(candidates)) {
        cJSON_Delete(root);
        return -1;
    }
    cJSON *first = cJSON_GetArrayItem(candidates, 0);
    if (!first) {
        cJSON_Delete(root);
        return -1;
    }
    cJSON *content = cJSON_GetObjectItemCaseSensitive(first, "content");
    if (!content) {
        cJSON_Delete(root);
        return -1;
    }
    cJSON *parts = cJSON_GetObjectItemCaseSensitive(content, "parts");
    if (!cJSON_IsArray(parts)) {
        cJSON_Delete(root);
        return -1;
    }
    cJSON *part0 = cJSON_GetArrayItem(parts, 0);
    if (!part0) {
        cJSON_Delete(root);
        return -1;
    }
    cJSON *text = cJSON_GetObjectItemCaseSensitive(part0, "text");
    if (!cJSON_IsString(text)) {
        cJSON_Delete(root);
        return -1;
    }
    strcpy(buf, text->valuestring);

    cJSON_Delete(root);
    return 0;
}

void send_prompt(const char *prompt, const char *sysprompt) {
    int fd[2];
    if (pipe(fd)) {
        fprintf(stderr, "Pipe failed.\n");
        exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Fork failed.\n");
        exit(1);
    }

    if (pid > 0) { // Parent
        close(fd[1]);
        char buffer[2048];
        size_t n;

        char response[2048];
        while ((n = read(fd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[n] = '\0';
            if (!parse_json_response(response, buffer)) {
                break;
            }
        }

        close(fd[0]);

        printf("%s\n", response);

        wait(NULL);
    } else { // Child
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        dup2(fd[1], STDERR_FILENO);
        close(fd[1]);
        execlp("./request.sh", "./request.sh", prompt, sysprompt, NULL);
        // If execlp fails
        perror("execlp");
        exit(1);
    }
}
