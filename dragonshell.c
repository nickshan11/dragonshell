#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include "jobs.h"

// Define process states
#define RUNNING 'R'
#define SUSPENDED 'T'


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
  printf("Welcome to DragonShell!\n\n");

  while (true) {
    printf("dragonshell> ");
    char input[1024];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;

    // tokenize the input
    char *tokens[100];
    tokenize(input, " ", tokens);

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
            fprintf(stderr, "Expected argument to \"cd\"\n");
        } else if (chdir(tokens[1]) != 0) {
            perror("cd");
        }
        continue;
    }

    // Jobs
    if (strcmp(tokens[0], "jobs") == 0) {
        print_jobs();
        continue;
    }

    // Exit
    if (strcmp(tokens[0], "exit") == 0) {
        terminate_all_processes(); // Gracefully terminate all processes
        printf("Exiting DragonShell. Goodbye!\n");
        break;
    }

    // External commands
    extern char **environ;

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        execve(tokens[0], tokens, environ);
        fprintf(stderr, "dragonshell: Command not found\n");
        _exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process
        add_process(pid, RUNNING, input); // Add process to the table
        int status;
        waitpid(pid, &status, 0);
        remove_process(pid); // Remove process after it terminates
    } else {
        perror("dragonshell");
    }
  }
  return 0;
}