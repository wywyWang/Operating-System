#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(){
    pid_t pid,pid_c1,pid_c2,wpid;
    int status=0;
    printf("Main process id : %d\n",getpid());
    pid=fork();
    if(pid==0){
        printf("Fork1, I'm the child %d, my parent is %d.\n",getpid(),getppid());
    }
    if(pid==0){
        pid_c1=fork();
        if(pid_c1==0)
            printf("Fork2, I'm the child %d, my parent is %d.\n",getpid(),getppid());
    }
    wait(NULL);
    pid_c2=fork();
    if(pid_c2==0)
        printf("Fork3, I'm the child %d, my parent is %d.\n",getpid(),getppid());
}
