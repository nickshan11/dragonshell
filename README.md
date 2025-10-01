# - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
# Name : Nick Shan
# SID : 1759726
# CCID : bshan1
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

## Design Choices:

### Built-in Commands:

The `getcwd()` system call was used to retrieve the current working directory, while `chdir()` was employed to change the directory. To gracefully terminate the shell, the `exit()` system call was utilized. 

For jobs, a linked list was used to make it easy to add new jobs and remove completed ones, keeping the job table flexible and able to adapt to changes quickly.

Built-in functions and Jobs recieved their own files to keep each file readable. Since the linked list implementation is quite long, it makes sense to put it in a seperate file and also helps with debugging. 

### Running External Programs:

The `fork()` system call creates a new child process to run external programs. This keeps the shell active and able to accept new commands while the child process runs.

The `execve()` system call replaces the child process's memory space with the external program. This lets the child process execute the program with the given arguments and environment variables.

The `waitpid()` system call ensures the shell waits for the child process to finish when running in the foreground. This prevents the shell from moving to the next command prematurely.

### Input & Output Redirection:

The `open()` system call is used to handle file input and output redirection. For input redirection (`<`), the file is opened in read-only mode using `O_RDONLY`. For output redirection (`>`), the file is opened in write-only mode with `O_WRONLY | O_CREAT | O_TRUNC`, ensuring the file is created if it does not exist or overwritten if it does.

The `dup2()` system call is used to redirect the standard input or output to the file descriptor of the opened file. By duplicating the file descriptor to `STDIN_FILENO` or `STDOUT_FILENO`, the shell ensures that the external program reads from or writes to the specified file.

The `close()` system call is used to release the file descriptor after redirection is set up. This prevents resource leaks and ensures that system resources are managed efficiently.

### Pipes:

The `pipe()` system call is used to create a unidirectional communication channel between two processes. It generates a pair of file descriptors: one for reading and one for writing. This allows the output of one process to be used as the input for another, enabling the implementation of pipelines.

Each process in the pipeline is created using the `fork()` system call. The first process writes to the pipe, while the second process reads from it. By forking separate processes, the shell ensures that each command in the pipeline runs independently and communicates through the pipe.

The `dup2()` system call is used to redirect the standard input or output of a process to the appropriate end of the pipe. By duplicating the pipe's file descriptors to `STDIN_FILENO` or `STDOUT_FILENO`, the shell connects the output of one process directly to the input of another, enabling effective communication between processes.

The `close()` system call is used to release the pipe's file descriptors after redirection is set up. This ensures that system resources are managed efficiently and prevents resource leaks.

### Signals:

The `sigaction()` system call is used to define custom handlers for signals like `SIGINT` (Ctrl-C) and `SIGTSTP` (Ctrl-Z). This allows the shell to intercept these signals and forward them to the appropriate child processes, ensuring the shell itself remains unaffected.

The `kill()` system call is used to send signals to specific processes. In the shell, it is used to forward signals like `SIGINT` or `SIGTSTP` to child processes, enabling proper termination or suspension of running commands.

The `waitpid()` system call is used to handle the termination of child processes. By waiting for specific child processes to exit, the shell ensures that resources are cleaned up and that signals are managed correctly.

### Running Background Processes:

For background processes, the `fork()` system call was used without `waitpid()`, allowing the shell to continue accepting new commands while the background process runs independently. Outputs from background processes get sent to `/dev/null` to prevent outputting to stdout.

### Testing:

The testing process began with running the sample tests provided in the assignment description. These tests were used to verify the basic functionality of the shell, including built-in commands, external program execution, file redirection, piping, and signal handling.

After the sample tests, additional edge cases were tested to ensure the robustness of the implementation. For example, edge cases included invalid commands, missing files for redirection, deeply nested pipelines, and handling signals like `Ctrl-C` and `Ctrl-Z` during various operations. 

## References:

Syscall commands from:<br>
https://linux.die.net/man/2/ <br>
https://man7.org/linux/man-pages/dir_section_2.html <br>

Linked list:<br>
https://www.geeksforgeeks.org/c/linked-list-in-c/<br>
https://medium.com/@Dev_Frank/c-linked-list-singly-linked-list-12aecb168834<br>

Pipes:<br>
https://tldp.org/LDP/lpg/node11.html<br>
https://www2.cs.uregina.ca/~hamilton/courses/330/notes/unix/pipes/pipes.html<br>
https://medium.com/@show981111/what-is-pipe-in-os-424cfc65532f<br>
https://www.scaler.com/topics/pipes-in-os/<br>
Week 2 TA Slides


File Redirection:<br>
https://www2.cs.uregina.ca/~hamilton/courses/330/notes/unix/filesyscalls.html<br>
https://www.geeksforgeeks.org/c/dup-dup2-linux-system-call/<br>

Signals:<br>
https://www.andrew.cmu.edu/course/15-310/applications/homework/homework4/signal_example2.html <br>
https://stackoverflow.com/questions/11465148/using-sigaction-c <br>
https://www.codequoi.com/en/sending-and-intercepting-a-signal-in-c/<br>
Week 2 TA Slides

