#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_LINE 80

int main(void)
{
	char *arg[MAX_LINE/2+1]; /*command line arguments*/
	int should_run = 1; /*flag to determine when to exit program*/
	char inputBuffer[MAX_LINE];

	memset(inputBuffer, '\0', sizeof(inputBuffer));

	while(should_run){
		int length,background=0,status=0;
		pid_t pid;
		/* TODO*/
		if(pid>0){
			signal (SIGCHLD,SIG_IGN);
		}
		
		printf("osh>");
		fflush(stdout);
		length = read(STDIN_FILENO, inputBuffer, MAX_LINE);
		length = strlen(inputBuffer);
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
		printf("Before,I'm the child %d, my parent is %d.\n",getpid(),getppid());
		if(pid==0){
			status=execvp(*arg,arg);
			if(status<0){
				// fprintf(stderr,"ERROR:Can't execute %s\n",arg[0]);
				return 0;
			}
		}
		if(background==0){
			sleep(1);
			wait(&status);
			kill(pid,SIGKILL);
			printf("HELLO!!\n");
		}
		printf("After,I'm the child %d, my parent is %d.\n",getpid(),getppid());
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

