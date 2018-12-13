// Student ID:0516075
// Name      :王威堯
// Date      : 2018/12/13
#include "bmpReader.h"
#include "bmpReader.cpp"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
using namespace std;

#define MYRED	2
#define MYGREEN 1
#define MYBLUE	0

int imgWidth, imgHeight;
int MEAN_FILTER_SIZE=9;
int SOBEL_FILTER_SIZE;
int FILTER_SCALE;
int *filter_gx, *filter_gy;

sem_t sem[1861][2631];

const char *inputfile_name[5] = {
	"input1.bmp",
	"input2.bmp",
	"input3.bmp",
	"input4.bmp",
	"input5.bmp"
};
const char *outputMed_name[5] = {
	"output1.bmp",
	"output2.bmp",
	"output3.bmp",
	"output4.bmp",
	"output5.bmp"
};


unsigned char *pic_in, *pic_grey, *pic_mean, *pic_gx, *pic_gy, *pic_sobel,*pic_final;

void *compute_mean(void* idx)
{
	for (int h = 0; h<imgHeight; h++) {
		for (int w = 0; w<imgWidth; w++){

			int tmp = 0;
			int a, b , window[9],k=0,sum=0;
			// int ws = (int)sqrt((float)MEAN_FILTER_SIZE);
			int ws=3;
			for (int j = 0; j<ws; j++)
			for (int i = 0; i<ws; i++)
			{
				a = w + i - (ws / 2);
				b = h + j - (ws / 2);

				// detect for borders of the image
				if (a<0 || b<0 || a>=imgWidth || b>=imgHeight)continue;

				sum=sum+pic_grey[b*imgWidth + a];
			};

			tmp=sum/MEAN_FILTER_SIZE;
			
			if (tmp < 0) tmp = 0;
			if (tmp > 255) tmp = 255;

			pic_mean[h*imgWidth + w] = tmp;
			sem_post(&sem[h][w]);	
			sem_post(&sem[h][w]);	
		}
	}
	pthread_exit(NULL);
}

unsigned char sobelFilter(int w, int h)
{
	int tmp = 0,tmp2=0;
	int a, b;
	// int ws = (int)sqrt((float)SOBEL_FILTER_SIZE);
	int ws=3;
	for (int j = 0; j<ws; j++)
	for (int i = 0; i<ws; i++)
	{
		a = w + i - (ws / 2);
		b = h + j - (ws / 2);

		// detect for borders of the image
		if (a<0 || b<0 || a>=imgWidth || b>=imgHeight) continue;

		tmp += filter_gx[j*ws + i] * pic_mean[b*imgWidth + a];
		tmp2 += filter_gy[j*ws + i] * pic_mean[b*imgWidth + a];
	};
	if (tmp < 0) tmp = 0;
	else if (tmp > 255) tmp = 255;
	if (tmp2 < 0) tmp2 = 0;
	else if (tmp2 > 255) tmp2 = 255;

	pic_gx[h*imgWidth + w]=tmp;
	pic_gy[h*imgWidth + w]=tmp2;

	tmp = 0;
	tmp = sqrt(pic_gx[h*imgWidth + w]*pic_gx[h*imgWidth + w] + pic_gy[h*imgWidth + w]*pic_gy[h*imgWidth + w]);
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;

	pic_sobel[h*imgWidth + w]=tmp;
}

void *compute_sobel(void* idx){

    //apply the gy_sobel filter to the image 
    for (int j = 0; j < imgHeight; j++){
        for (int i = 0; i < imgWidth; i++){
            if(j == imgHeight-1 && i == imgWidth-1)sem_wait(&sem[j][i]);
            else if(i == imgWidth-1)sem_wait(&sem[j+1][i]);
            else if(j == imgHeight-1)sem_wait(&sem[j][i+1]);
            else sem_wait(&sem[j+1][i+1]);
			sobelFilter(i, j);
			pic_final[3 * (j*imgWidth + i) + MYRED] = pic_sobel[j*imgWidth + i];
			pic_final[3 * (j*imgWidth + i) + MYGREEN] = pic_sobel[j*imgWidth + i];
			pic_final[3 * (j*imgWidth + i) + MYBLUE] = pic_sobel[j*imgWidth + i];
			if(j == imgHeight-1 && i == imgWidth-1)sem_post(&sem[j][i]);
            else if(i == imgWidth-1)sem_post(&sem[j+1][i]);
            else if(j == imgHeight-1)sem_post(&sem[j][i+1]);
            else sem_post(&sem[j+1][i+1]);
        }
    }
    pthread_exit(NULL);
}


int main()
{
	// read mask file
	FILE* mask;
    pthread_t t1,t2;
    int id[1];
	id[0]=1;

	mask = fopen("mask_Sobel.txt", "r");
	fscanf(mask, "%d", &SOBEL_FILTER_SIZE);

	filter_gx = new int[SOBEL_FILTER_SIZE];
	filter_gy = new int[SOBEL_FILTER_SIZE];

	for (int i = 0; i<SOBEL_FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_gx[i]);

	for (int i = 0; i<SOBEL_FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_gy[i]);

	fclose(mask);
	
	BmpReader* bmpReader = new BmpReader();
	for (int k = 0; k<5; k++){

		// read input BMP file
		pic_in = bmpReader->ReadBMP(inputfile_name[k], &imgWidth, &imgHeight);
		// allocate space for output image
		pic_grey = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_mean = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_gx = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_gy = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_sobel = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
		pic_final = (unsigned char*)malloc(3 * imgWidth*imgHeight*sizeof(unsigned char));

		for (int h = 0; h<imgHeight; h++) {
			for (int w = 0; w<imgWidth; w++){
				sem_init(&sem[h][w],0,0);
				int tmp = (
				pic_in[3 * (h*imgWidth + w) + MYRED] +
				pic_in[3 * (h*imgWidth + w) + MYGREEN] +
				pic_in[3 * (h*imgWidth + w) + MYBLUE] )/3;

				if (tmp < 0) tmp = 0;
				if (tmp > 255) tmp = 255;
				pic_grey[h*imgWidth + w] = tmp;
			}
		}

        pthread_create(&t1,NULL,compute_mean,(void*) &id[0]);
        pthread_create(&t2,NULL,compute_sobel,(void*) &id[0]);
		
        pthread_join(t1,NULL);
        pthread_join(t2,NULL);

		bmpReader->WriteBMP(outputMed_name[k], imgWidth, imgHeight, pic_final);

		//free memory space
		free(pic_in);
		free(pic_grey);
		free(pic_mean);
		free(pic_final);
		free(pic_sobel);
		free(pic_gx);
		free(pic_gy);
	}

	return 0;
}