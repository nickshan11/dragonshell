#include "jobs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

// Head of the process table
Process *process_table = NULL;

// Function to add a process to the process table
void add_process(pid_t pid, char state, const char *command) {
    Process *new_process = malloc(sizeof(Process));
    new_process->pid = pid;
    new_process->state = state;
    strncpy(new_process->command, command, sizeof(new_process->command) - 1);
    new_process->command[sizeof(new_process->command) - 1] = '\0';
    new_process->next = process_table;
    process_table = new_process;
}

// Function to remove a process from the process table
void remove_process(pid_t pid) {
    Process *current = process_table;
    Process *prev = NULL;
    while (current != NULL) {
        if (current->pid == pid) {
            if (prev == NULL) {
                process_table = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}

// Function to update the state of a process in the process table
void update_process_state(pid_t pid, char state) {
    Process *current = process_table;
    while (current != NULL) {
        if (current->pid == pid) {
            current->state = state;
            return;
        }
        current = current->next;
    }
}

// Function to print the process table (jobs command)
void print_jobs() {
    Process *current = process_table;
    while (current != NULL) {
        printf("%d %c %s\n", current->pid, current->state, current->command);
        current = current->next;
    }
}

// Function to terminate all processes in the process table
void terminate_all_processes() {
    Process *current = process_table;
    while (current != NULL) {
        kill(current->pid, SIGTERM); // Send SIGTERM to terminate the process
        Process *temp = current;
        current = current->next;
        free(temp); // Free the process table entry
    }
    process_table = NULL; // Reset the process table
}
