#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include "jobs.h"
#include "builtins.h"

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

    // Built-in commands
    // pwd
    if (strcmp(tokens[0], "pwd") == 0) {
        builtin_pwd();
        continue;
    }
    // cd
    if (strcmp(tokens[0], "cd") == 0) {
        builtin_cd(tokens[1]);
        continue;
    }
    // jobs
    if (strcmp(tokens[0], "jobs") == 0) {
        print_jobs();
        continue;
    }
    // exit
    if (strcmp(tokens[0], "exit") == 0) {
        terminate_all_processes(); // Gracefully terminate all processes
        builtin_exit();
    }

    // External commands
    extern char **environ;

    // Check if the command ends with '&'
    bool is_background = false;
    int token_count = 0;
    while (tokens[token_count] != NULL) token_count++;
    if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) {
        is_background = true;
        tokens[token_count - 1] = NULL; // Remove '&' from the tokens
    }

    // Check for input/output redirection
    char *input_file = NULL;
    char *output_file = NULL;
    for (int i = 0; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "<") == 0) {
            input_file = tokens[i + 1];
            tokens[i] = NULL; // Split the command here
        } else if (strcmp(tokens[i], ">") == 0) {
            output_file = tokens[i + 1];
            tokens[i] = NULL; // Split the command here
        }
    }

    // Check for piping
    char *cmd1[100], *cmd2[100];
    bool is_pipe = false;
    int pipe_index = -1;
    for (int i = 0; tokens[i] != NULL; i++) {
        if (strcmp(tokens[i], "|") == 0) {
            is_pipe = true;
            pipe_index = i;
            tokens[i] = NULL; // Split the command here
            break;
        }
    }

    if (is_pipe) {
        // Split tokens into cmd1 and cmd2
        for (int i = 0; i < pipe_index; i++) {
            cmd1[i] = tokens[i];
        }
        cmd1[pipe_index] = NULL;

        int j = 0;
        for (int i = pipe_index + 1; tokens[i] != NULL; i++) {
            cmd2[j++] = tokens[i];
        }
        cmd2[j] = NULL;

        // Create a pipe
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("dragonshell: Pipe failed");
            continue;
        }

        // Fork for the first command
        pid_t pid1 = fork();
        if (pid1 == 0) {
            // Child process for cmd1
            close(pipefd[0]); // Close unused read end
            dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to pipe
            close(pipefd[1]);
            execve(cmd1[0], cmd1, environ);
            perror("dragonshell: Command not found");
            _exit(EXIT_FAILURE);
        }

        // Fork for the second command
        pid_t pid2 = fork();
        if (pid2 == 0) {
            // Child process for cmd2
            close(pipefd[1]); // Close unused write end
            dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to pipe
            close(pipefd[0]);
            execve(cmd2[0], cmd2, environ);
            perror("dragonshell: Command not found");
            _exit(EXIT_FAILURE);
        }

        // Parent process
        close(pipefd[0]);
        close(pipefd[1]);
        waitpid(pid1, NULL, 0); // Wait for cmd1 to finish
        waitpid(pid2, NULL, 0); // Wait for cmd2 to finish
    } else {
        // Fork the process for a single command
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            execve(tokens[0], tokens, environ);
            perror("dragonshell: Command not found");
            _exit(EXIT_FAILURE);
        } else if (pid > 0) {
            // Parent process
            if (is_background) {
                printf("PID %d is sent to background\n", pid);
                add_process(pid, RUNNING, input); // Add to process table
            } else {
                int status;
                waitpid(pid, &status, 0); // Wait for foreground process
                remove_process(pid); // Remove from process table
            }
        } else {
            // Fork failed
            perror("dragonshell");
        }
    }
  }
  return 0;
}