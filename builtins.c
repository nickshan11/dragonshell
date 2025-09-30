#include "builtins.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

void builtin_pwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("pwd");
    }
}

void builtin_cd(char *path) {
    if (path == NULL) {
        fprintf(stderr, "Expected argument to \"cd\"\n");
    } else if (chdir(path) != 0) {
        perror("cd");
    }
}

void builtin_exit() {
    printf("Exiting DragonShell. Goodbye!\n");
    exit(0);
}