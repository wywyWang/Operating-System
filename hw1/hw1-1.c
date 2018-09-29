#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80

int main(void)
{
	char *arg[MAX_LINE/2+1]; /*command line arguments*/
	int should_run = 1; /*flag to determine when to exit program*/
	char inputBuffer[MAX_LINE];

	memset(inputBuffer, '\0', sizeof(inputBuffer));

	while(should_run){
		int length,background=0,status;
		pid_t pid;
		printf("osh>");
		fflush(stdout);
		length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
		length = strlen(inputBuffer);
		// printf("len = %d\n",length);
		// printf("inputbuffer = %s\n",inputBuffer);
		int start=-1,cnt=0;
		for(int i=0;i<length;i++){
			switch(inputBuffer[i]){
				case ' ':
				case '\t':
					if(start!=-1){
						arg[cnt]=&inputBuffer[start];
						cnt++;
					}
					inputBuffer[i]='\0';
					start=-1;
					break;
				case '\n':
					if(start!=-1){
						arg[cnt]=&inputBuffer[start];
						cnt++;
					}
					inputBuffer[i]='\0';
					arg[cnt]=NULL;
					break;
				case '&':
					background=1;
					inputBuffer[i]='\0';
					break;
				default:
					if(start==-1){
						start=i;
					}
			}
			arg[cnt]=NULL;
		}
		fflush(stdout);
		if(strcmp(*arg,"exit")==0){
			should_run=0;
			break;
		}
		pid=fork();
		if(pid==0){
			status=execvp(*arg,arg);
			if(status<0){
				// fprintf(stderr,"ERROR:Can't execute %s\n",arg[0]);
				exit(1);
			}
		}
		else if(background==0){
			wait(NULL);
		}
		/**
		* your code!
		* After reading user input, the step are:
		* (1) fork a child process using fork()
		* (2) the child process will invoke execvp()
		* (3) if command included &, parent will invoke wait()
		*/
	}

	return 0;
}

