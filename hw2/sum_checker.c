#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUMBER_OF_THREADS 11 /* Hint */
#define PUZZLE_SIZE 9

/* example puzzle */
int puzzle[PUZZLE_SIZE+1][PUZZLE_SIZE+1] = {
			{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0},
			{-1,0,0,0,0,0,0,0,0,0}
		};

/* data structure for passing data to threads */
typedef struct
{
	int thread_number;
	int x;
	int y;
} parameters;

/* print puzzle */ 
#define LINE "====================== \n"
#define COLLINE "\n___________________________________ \n"
#define NAME "||  SUM   CHECKER  || \n"
void print_grid(int grid[10][10])
{
    int i,j;
    printf(LINE);
    printf(NAME);
    printf(LINE);

	for (i = 1; i < 10; i++)
	{
		for (j = 1; j < 10; j++)
		{
	        printf("|%1d |",grid[i][j]);
		}
        printf(COLLINE);
	}
    printf("\n");
}
// read file to check sudoku
void SetPuzzle(char filename[]){
    FILE *file = fopen(filename,"r");
    int i,j,ch,num;
    for (i=0;i<=9;i++){
        for(j=0;j<=9;j++){
            while(((ch = getc(file)) != EOF)){
                if(ch == EOF) break;
                if(ch == ',') break;
                if(ch == '\n') break;
                ungetc(ch,file);
                fscanf(file,"%d",&num);
                if(num!=-1)
                    puzzle[i][j] = num;            
            } 
        }
    }
    print_grid(puzzle);
    return;
}

int total[30]={0};

void *compute_row(void* idx){
    for(int i=1;i<10;i++){
        int sum=0;
        for(int j=1;j<10;j++){
            total[i-1]+=puzzle[i][j];
        }
    }
    pthread_exit(NULL);
}

void *compute_col(void* idx){
    for(int i=1;i<10;i++){
        int sum=0;
        for(int j=1;j<10;j++){
            total[i+9-1]+=puzzle[j][i];
        }
    }
    pthread_exit(NULL);
}

void *compute_grid1(void* idx){
    for(int i=1;i<4;i++){
        for(int j=1;j<4;j++){
            total[18]+=puzzle[i][j];
        }
    }
    pthread_exit(NULL);
}

void *compute_grid2(void* idx){
    for(int i=1;i<4;i++){
        for(int j=4;j<7;j++){
            total[19]+=puzzle[i][j];
        }
    }
    pthread_exit(NULL);
}

void *compute_grid3(void* idx){
    for(int i=1;i<4;i++){
        for(int j=7;j<10;j++){
            total[20]+=puzzle[i][j];
        }
    }
    pthread_exit(NULL);
}

void *compute_grid4(void* idx){
    for(int i=4;i<7;i++){
        for(int j=1;j<4;j++){
            total[21]+=puzzle[i][j];
        }
    }
    pthread_exit(NULL);
}

void *compute_grid5(void* idx){
    for(int i=4;i<7;i++){
        for(int j=4;j<7;j++){
            total[22]+=puzzle[i][j];
        }
    }
    pthread_exit(NULL);
}

void *compute_grid6(void* idx){
    for(int i=4;i<7;i++){
        for(int j=7;j<10;j++){
            total[23]+=puzzle[i][j];
        }
    }
    pthread_exit(NULL);
}

void *compute_grid7(void* idx){
    for(int i=7;i<10;i++){
        for(int j=1;j<4;j++){
            total[24]+=puzzle[i][j];
        }
    }
    pthread_exit(NULL);
}

void *compute_grid8(void* idx){
    for(int i=7;i<10;i++){
        for(int j=4;j<7;j++){
            total[25]+=puzzle[i][j];
        }
    }
    pthread_exit(NULL);
}

void *compute_grid9(void* idx){
    for(int i=7;i<10;i++){
        for(int j=7;j<10;j++){
            total[26]+=puzzle[i][j];
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
	int rv = 1; // flag to check answer
    // input the sudoku file
    SetPuzzle("test.txt");
    
    pthread_t row,col,grid;

    int id[1];
    id[0]=1;
    pthread_create(&row,NULL,compute_row,(void*) &id[0]);
    pthread_create(&col,NULL,compute_col,(void*) &id[0]);

    int id2[10];
    for(int i=1;i<10;i++){
        id2[i]=i;
    }
    pthread_create(&grid,NULL,compute_grid1,(void*) &id2[1]);
    pthread_create(&grid,NULL,compute_grid2,(void*) &id2[2]);
    pthread_create(&grid,NULL,compute_grid3,(void*) &id2[3]);
    pthread_create(&grid,NULL,compute_grid4,(void*) &id2[4]);
    pthread_create(&grid,NULL,compute_grid5,(void*) &id2[5]);
    pthread_create(&grid,NULL,compute_grid6,(void*) &id2[6]);
    pthread_create(&grid,NULL,compute_grid7,(void*) &id2[7]);
    pthread_create(&grid,NULL,compute_grid8,(void*) &id2[8]);
    pthread_create(&grid,NULL,compute_grid9,(void*) &id2[9]);
    pthread_join(row,NULL);
    pthread_join(col,NULL);
    pthread_join(grid,NULL);
    int cnt=0;
    for(int i=0;i<26;i++){
        //printf("total %d = %d\n",i,total[i]);
        if(total[i]==total[i+1]){
            cnt++;
        }
    }
    //printf("cnt = %d\n",cnt);
    if(cnt==26){
        rv=1;
    }
    else{
        rv=0;
    }
    
	if (rv == 1)
		printf("Successful :) \n");
	else
		printf("Must check again! :( \n");
    sleep(5);
	return 0;
}
