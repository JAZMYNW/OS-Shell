#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include "myshell_parser.h"


void SHchild(){
	int status;
	while(1){
		pid_t pid = waitpid(-1,&status,WNOHANG);

	}
}
int commandex(struct pipeline_command *mycommand, int fdin, int fdout){
	if(mycommand->redirect_in_path!=NULL){
		int in = open(mycommand->redirect_in_path, O_RDONLY);
		if(in == -1){
			perror("ERROR:File cannot be opened!");
			
		}
		dup2(in, STDIN_FILENO);
		close(in);
	} else if(fdin != STDIN_FILENO){
		dup2(fdin,STDIN_FILENO);
		close(fdin);
	}
	if(mycommand->redirect_out_path!=NULL){
		int out = creat(mycommand->redirect_out_path, S_IRWXU);
		if (out == -1) {
                perror("ERROR: file cannot be created");
                
            }
		dup2(out, STDOUT_FILENO);
        close(out);
	} else if(fdout != STDOUT_FILENO){
		dup2(fdout,STDOUT_FILENO);
		close(fdout);
	}
	if(execvp(mycommand->command_args[0],mycommand->command_args)==-1){
		perror("ERROR:Command Argument Execution failed!");
		
	}

return 0;
}
int pipelineex(struct pipeline_command *mycommand, int fdin, int fdout, bool background) {
    if (mycommand == NULL) {
        return 0;
    }

    pid_t pid;
    int fd[2];

    if (pipe(fd) == -1) {
        perror("ERROR: Pipe creation failed!");
        return -1;
    }

    pid = fork();

    if (pid == -1) {
        perror("ERROR: Fork Failed!");
        return -1;
    } else if (pid == 0) { /* Child */
        if (fdin != STDIN_FILENO) {
            close(STDIN_FILENO);
            dup2(fdin, STDIN_FILENO);
            close(fdin);
        }

        if (mycommand->next != NULL) {
            close(STDOUT_FILENO);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);
        } else if (fdout != STDOUT_FILENO) {
            close(STDOUT_FILENO);
            dup2(fdout, STDOUT_FILENO);
            close(fdout);
        }

        close(fd[0]);
        
        // Execute the current command
        commandex(mycommand, STDIN_FILENO, STDOUT_FILENO);

        // Exit the child process
        exit(0);
    } else /* Parent */ {
        close(fd[1]);
        if (fdin != STDIN_FILENO) {
            close(fdin);
        }
        if (!background) {
            int status;
            waitpid(pid, &status, 0);
        }
        if (mycommand->next != NULL) {
            //printf("Executing command: %s\n", mycommand->command_args[0]);
           mycommand = mycommand->next;

    // Recursively execute the next command
    close(fd[1]); // Close the write end of the pipe
    pipelineex(mycommand, fd[0], fdout, background);
}
    }

    return 0;
}


int main(int argc, char *argv[]){

char *prompt = "my_shell$ ";
char *userin;
char line[MAX_LINE_LENGTH];
struct pipeline* my_pipeline;
bool background;

	
  while(1){
	if((argc > 1) && (strcmp(argv[1], "-n") == 0)){
		//no prompt	
	}else{
		printf("%s",prompt); //prompt
	}

		if((userin = fgets(line, MAX_LINE_LENGTH, stdin))==NULL)
			break; //ctrl-d
		else{
			int fdin = STDIN_FILENO;
			int fdout = STDOUT_FILENO;
			my_pipeline = pipeline_build(userin);//calling  pipeline build
			struct pipeline_command* current = my_pipeline->commands;
			background = my_pipeline->is_background;
			
			if(my_pipeline != NULL){
				
					if(pipelineex(current,fdin,fdout,background)!= 0){
						perror("ERROR:Execution Failed");
					}
					
				

			}else {
				perror("ERROR: Incomplete Build");
			}

          
        
			
        }
		
    }
	pipeline_free(my_pipeline);
	return 0;

}



