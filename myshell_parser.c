#include "myshell_parser.h"
#include "stddef.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int l; //size of the line of text
char word[20];
char curr;
int commandcount = 0;
char *str;

struct pipeline_command *pcallocate(){
	struct pipeline_command *pc = (struct pipeline_command*) malloc(sizeof(struct pipeline_command));
		for(int i = 0; i < MAX_ARGV_LENGTH; i++){
			pc->command_args[i]=NULL;
		}
		pc->next = NULL;
		pc->redirect_in_path = NULL;
		pc->redirect_out_path = NULL;

		return pc;
}

void fill_commargs(struct pipeline_command *pc1, char *comm){
if(strchr(comm, '\n')){
	comm = strtok(comm,"\n");
} 
 char *token = strdup(comm);
 char *token2 = strdup(comm);
 char *token3 = strdup(comm);
 int index;
 bool redirecti = false;
 bool redirecto = false;

 
  char *redin = "<";
  char *redout = ">";
 
 int cpos = 0;
 
 if(strchr(token,'<')){
   token =  strtok(token,"<");
   token =  strtok(NULL,"<");
        if(strchr(token,'>')){
           token =  strtok(token,">");
        }
            
        if(strchr(token2,' ')||strchr(token2,'\n')||strchr(token2,'\t')){
           token =  strtok(token," ");
            
    }
     
      pc1->redirect_in_path = strdup(token);
      redirecti = true;
 }
 

 
 if(strchr(token2,'>')){
 
    token2 = strtok(token2,">");
     
    
   token2 =  strtok(NULL,">");
  
         
        if(strchr(token2,'<')){
             
            
            token2 = strtok(token2,"<");
        }
            
        if(strchr(token2,' ')||strchr(token2,'\n')||strchr(token2,'\t')){
            
            
           token2 =  strtok(token2," ");
	   
           
    }
     
      pc1->redirect_out_path = strdup(token2);
      redirecto = true;
 }
 
  
    token3 = strtok(token3, " ");
    while (token3 != NULL) {
        
	if( ((strcmp(token3,token2)==0) && redirecto)  || ((strcmp(token3,token)==0) && redirecti == true)){
	goto end;
	}

        if((redin = strchr(token3,'<'))){
            char token1[52];
	    strcpy(token1,token3);
	    index = (int)(redin - token3);
	    token1[index] = '\0';
	    if(!strlen(token1)){
	    	goto end;
	    }
	    token3 = token1;
	    
	    
	    
        }
         if((redout = strchr(token3,'>'))){
            char token1[52];
	    strcpy(token1,token3);
	    index = (int)(redout - token3);
	    token1[index] = '\0';
	    if(!strlen(token1)){
	    	goto end;
	    }
	    token3 = token1;
	   
	    
	    
        }
      
        
       pc1->command_args[cpos] = strdup(token3);
        cpos++;
        
end:  
        token3 = strtok(NULL, " ");
    }
        
     
        token3 = strtok(NULL, " ");
    }
    
   



struct pipeline *pipeline_build(const char *command_line){
	
      
	char *editline = strdup(command_line); //for use as to not lose orginal data
	char *currcom;
	
	if( command_line == NULL ||*command_line == '\0'){
		return NULL;
	}

	//dynamically allocating pipeline, initializing is_background member
	struct pipeline *p = (struct pipeline *) malloc(sizeof(struct pipeline));
		p->is_background = 0;
		p->commands = NULL;
	//

	//checking for background;
	if(strchr(editline,'&')) {
		strtok(editline,"&");
		p->is_background = true;
	
	}

	//checking for piping
	if(strchr(editline,'|')) {
		
	struct pipeline_command *current = NULL;

		 while ((currcom = strtok_r(editline, "|", &editline))){
		 	struct pipeline_command *nextc = pcallocate();
		 	 fill_commargs(nextc, currcom);
		 

			if (current) {
				current->next = nextc;
			} else {
				p->commands = nextc;
			}
			current = nextc;
			
		 
		 }	
	
	} 
	else {
		
		p->commands = pcallocate();
		fill_commargs(p->commands, editline);
	
	

		}
	
	
	
	
	 
	return p;
	 

}

void pipeline_free(struct pipeline *pipeline)
{
	//must free pipeline_command first
	struct pipeline_command *current = pipeline->commands;
	while (current != NULL) {
		struct pipeline_command *next = current->next;
		for (int i = 0; i < MAX_ARGV_LENGTH && current->command_args[i] != NULL; i++) {
    		free(current->command_args[i]);
		}
		if (current->redirect_in_path != NULL) {
			free(current->redirect_in_path);
		}
		if (current->redirect_out_path != NULL) {
			free(current->redirect_out_path);
		}
		free(current);
		current = next;
	}
	//freeing actual pipeline
	free(pipeline);
}
