Shell Parser:
The shell parser consists of a helper function fill_commargs to assist in filling in the pipeline struct created with the pipeline build function.

Fill_commargs: This function is used to parse the commands based on the actual characters themselves. This allows the parser to work the same despite varying whitespace between arguments. I used variables labeled token1, token2, and token3 to keep track of the arguments found in the parsing process. Tokens 1 and 2 handle the redirection path and help fill that piece of the struct. Token 3 is responsible for separating the rest of the arguments in the line. We begin by checking whether or not the the current argument matches either of the already found redirection paths, if it does we jump to the end statement to break out of the loop. In order to avoid processing it as another command. Because each token contains the original line if the redirection symbol is detected in the final check, we have to strip the character and check if there's any useful argument following which is done using token1. In its entirety, this function fills the command portion of the pipeline.

Pipeline_build: Pipeline_build works by calling fill comm_args to build a linked list for the command member of the struct. It begins by checking the passed input for the background character. If detected we set the boolean member to true. It then checks the line for the pipe character and passes each portion to the fill_commargs function to fill out the commands.


Execution:

Main: In the main function the pipeline_build function is called with the user input string as an argument. The command list of the pipeline is stored in the current variable, and the background flag is set based on the pipeline. To begin the execution of the arguments, pipeex is called which takes in the input file descriptor, the output file descriptor, the background flag, and the current set of commands. There are various if statements for the purpose of error checking.

Pipeex:In the child process redirects standard input to the fdin file descriptor is not the default standard input. This is done to read input from a file or a previous command's output via a pipe. Then it checks if the current command (mycommand) has a "next" command in the pipeline 
If there's a next command, it redirects the standard output (STDOUT_FILENO) to the write end of the pipe (fd[1]) to pass its output to the next command.
If there's no next command, it redirects the standard output to fdout if it's not the default standard output. This allows the output of the command to be directed to a file or another command via a pipe. Commandex is then called within the pipeex to execute the current command.In the parent process, the function closes the write end of the pipe (fd[1]) because it's not needed by the parent. Checks if fdin is not the default standard input and closes it.
It waits for the child process to complete using waitpid. This ensures that the parent process waits for the child to finish executing the command before proceeding. In order to proceed executing the pipeline the function calls itself recursively.

Commandex: This function is used to handle the redirections and actual execution of the commands. This function takes in the input and output file descriptors.
Redirect In
The function first checks if the redirect_in_path member of mycommand is not NULL
If input redirection is specified, the function attempts to open the file specified in mycommand->redirect_in_path for reading only
If the file open operation fails (i.e., in is set to -1), it prints an error message using perror, indicating that the file cannot be opened.
If the file is successfully opened, the function uses dup2 to redirect the file descriptor in to the standard input (STDIN_FILENO) to make the command read from the specified file.
The in file descriptor is then closed because it's no longer needed after redirection.
Redirect Out
The function checks if output redirection is specified for this command.
The function attempts to create the file specified with read, write, and execute permissions for the owner (S_IRWXU).
If the file creation operation fails (i.e., out is set to -1), it prints an error message using perror, indicating that the file cannot be created.
If the file is successfully created, the function uses dup2 to redirect the file descriptor out to the standard output (STDOUT_FILENO), effectively making the command's output go to the specified file.
The out file descriptor is then closed because it's no longer needed after redirection.
FD’s other than standard
the function checks if the fdin and fdout variables are not default standard output  This condition implies that the command's output should be directed to another command in the pipeline via a pipe. 
The function then uses exec.


