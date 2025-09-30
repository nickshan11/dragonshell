#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>

// Define process states
#define RUNNING 'R'
#define SUSPENDED 'T'

// Define the process table entry structure
typedef struct Process {
    pid_t pid;
    char state;
    char command[1024];
    struct Process *next;
} Process;

void add_process(pid_t pid, char state, const char *command);
void remove_process(pid_t pid);
void update_process_state(pid_t pid, char state);
void print_jobs();
void terminate_all_processes();

#endif
