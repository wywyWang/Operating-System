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
int SOBEL_FILTER_SIZE;
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


unsigned char *pic_in,*pic_final;

unsigned char pic_grey[1861*2631],pic_mean[1861*2631],pic_gx[1861*2631],pic_gy[1861*2631],pic_sobel[1861*2631];

void *compute_mean(void* idx)
{
	for (int h = 0; h<imgHeight; h++) {
		for (int w = 0; w<imgWidth; w++){

			int tmp = 0;
			int a, b ,k=0,sum=0;
			for (int j = 0; j<3; j++){
				a = w + 0 - 1;
				b = h + j - 1;
				// detect for borders of the image
				if (a>=0 && b>=0 && a<imgWidth && b<imgHeight)
					sum=sum+pic_grey[b*imgWidth + a];

				a = w + 1 - 1;
				b = h + j - 1;
				// detect for borders of the image
				if (a>=0 && b>=0 && a<imgWidth && b<imgHeight)
					sum=sum+pic_grey[b*imgWidth + a];

				a = w + 2 - 1;
				b = h + j - 1;
				// detect for borders of the image
				if (a>=0 && b>=0 && a<imgWidth && b<imgHeight)
					sum=sum+pic_grey[b*imgWidth + a];
			}

			tmp=sum/9;
			
			if (tmp < 0) tmp = 0;
			if (tmp > 255) tmp = 255;

			pic_mean[h*imgWidth + w] = tmp;
			sem_post(&sem[h][w]);	
			sem_post(&sem[h][w]);	
		}
	}
	pthread_exit(NULL);
}

void * compute_sobel(void* idx)
{
	for (int h = 0; h < imgHeight; h++){
        for (int w = 0; w < imgWidth; w++){
            if(h == imgHeight-1 && w == imgWidth-1)sem_wait(&sem[h][w]);
            else if(w == imgWidth-1)sem_wait(&sem[h+1][w]);
            else if(h == imgHeight-1)sem_wait(&sem[h][w+1]);
            else sem_wait(&sem[h+1][w+1]);
			
			int tmp = 0,tmp2=0;
			int a, b;
			for (int j = 0; j<3; j++)
			for (int i = 0; i<3; i++)
			{
				a = w + i - 1;
				b = h + j - 1;

				// detect for borders of the image
				if (a<0 || b<0 || a>=imgWidth || b>=imgHeight) continue;

				tmp += filter_gx[j*3 + i] * pic_mean[b*imgWidth + a];
				tmp2 += filter_gy[j*3 + i] * pic_mean[b*imgWidth + a];
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

			pic_final[3 * (h*imgWidth + w) + MYRED] = pic_sobel[h*imgWidth + w];
			pic_final[3 * (h*imgWidth + w) + MYGREEN] = pic_sobel[h*imgWidth + w];
			pic_final[3 * (h*imgWidth + w) + MYBLUE] = pic_sobel[h*imgWidth + w];
			if(h == imgHeight-1 && w == imgWidth-1)sem_post(&sem[h][w]);
            else if(w == imgWidth-1)sem_post(&sem[h+1][w]);
            else if(h == imgHeight-1)sem_post(&sem[h][w+1]);
            else sem_post(&sem[h+1][w+1]);
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

	// read input BMP file
	pic_in = bmpReader->ReadBMP(inputfile_name[0], &imgWidth, &imgHeight);
	// allocate space for output image
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

	bmpReader->WriteBMP(outputMed_name[0], imgWidth, imgHeight, pic_final);

	//free memory space
	free(pic_in);
	free(pic_final);

	// read input BMP file
	pic_in = bmpReader->ReadBMP(inputfile_name[1], &imgWidth, &imgHeight);
	// allocate space for output image
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

	bmpReader->WriteBMP(outputMed_name[1], imgWidth, imgHeight, pic_final);

	//free memory space
	free(pic_in);
	free(pic_final);

	// read input BMP file
	pic_in = bmpReader->ReadBMP(inputfile_name[2], &imgWidth, &imgHeight);
	// allocate space for output image
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

	bmpReader->WriteBMP(outputMed_name[2], imgWidth, imgHeight, pic_final);

	//free memory space
	free(pic_in);
	free(pic_final);

	// read input BMP file
	pic_in = bmpReader->ReadBMP(inputfile_name[3], &imgWidth, &imgHeight);
	// allocate space for output image
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

	bmpReader->WriteBMP(outputMed_name[3], imgWidth, imgHeight, pic_final);

	//free memory space
	free(pic_in);
	free(pic_final);

	// read input BMP file
	pic_in = bmpReader->ReadBMP(inputfile_name[4], &imgWidth, &imgHeight);
	// allocate space for output image
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

	bmpReader->WriteBMP(outputMed_name[4], imgWidth, imgHeight, pic_final);

	//free memory space
	free(pic_in);
	free(pic_final);

	return 0;
}