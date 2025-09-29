#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

/**
 * @brief Tokenize a C string 
 * 
 * @param str - The C string to tokenize 
 * @param delim - The C string containing delimiter character(s) 
 * @param argv - A char* array that will contain the tokenized strings
 * Make sure that you allocate enough space for the array.
 */
void tokenize(char* str, const char* delim, char ** argv) {
    char* token;
    size_t i = 0;
    token = strtok(str, delim);
    while (token != NULL) {
        argv[i++] = token;
        token = strtok(NULL, delim);
    }
    argv[i] = NULL; 
}

int main(int argc, char **argv) {
  // print the string prompt without a newline, before beginning to read
  // tokenize the input, run the command(s), and print the result
  // do this in a loop
  printf("Welcome to DragonShell!\n");


  while (true) {
    printf("dragonshell> ");
    char input[1024];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    // tokenize the input
    char *tokens[100];
    tokenize(input, " ", tokens);

    // [DEBUG] Print the tokens
    // printf("Tokens:\n");
    // for (size_t i = 0; tokens[i] != NULL; ++i) {
    //     printf("Token[%zu]: %s\n", i, tokens[i]);
    // }

    // default commands:
    // pwd
    if (strcmp(tokens[0], "pwd") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("pwd");
        }
        continue;
    }
    // cd <directory>
    if (strcmp(tokens[0], "cd") == 0) {
        if (tokens[1] == NULL) {
            fprintf(stderr, "cd: missing argument\n");
        } else if (chdir(tokens[1]) != 0) {
            perror("cd");
        }
        continue;
    }
    if (strcmp(tokens[0], "jobs") == 0) {
        printf("Jobs command not implemented yet.\n");
        continue;
    }
    if (strcmp(tokens[0], "exit") == 0) {
        printf("Not fully implemented yet.\n");
        continue;
    }

    // External commands
    extern char **environment; // Access the current environment

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (strchr(tokens[0], '/') == NULL) {
            // Command does not contain a path, search in PATH
            char *path = getenv("PATH");
            if (path) {
                char *dir = strtok(path, ":");
                char full_path[1024];
                while (dir) {
                    snprintf(full_path, sizeof(full_path), "%s/%s", dir, tokens[0]);
                    execve(full_path, tokens, environment);
                    dir = strtok(NULL, ":");
                }
            }
        } else {
            // Command contains a path
            execve(tokens[0], tokens, environment);
        }
        perror("dragonshell");
        _exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    } else {
        // Fork failed
        perror("dragonshell");
    }

  }
  return 0;
}