// Student ID:
// Name      :
// Date      : 2017.11.03

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
#define NUM_THREADS 5
#define NUM_THREADS_SMALL 5

int FILTER_SIZE;
int FILTER_SCALE;
int *filter_Gx;
int *filter_Gy;

const char *inputfile_name[5] = {
	"input1.bmp",
	"input2.bmp",
	"input3.bmp",
	"input4.bmp",
	"input5.bmp"
};

const char *outputSobel_name[5] = {
	"Sobel1.bmp",
	"Sobel2.bmp",
	"Sobel3.bmp",
	"Sobel4.bmp",
	"Sobel5.bmp"
};

struct thread_data {
   int  i,j;
   int* filter;
   pthread_mutex_t* mutex;
   int* pic_tmp;
   unsigned char* pic_grey;
   int imgWidth;
   int imgHeight;
};

struct file_exchange {
	int id;
	int imgWidth;
	int imgHeight;
	int* pic_tmp;
	unsigned char* pic_grey;
};

struct convert_data {
   	unsigned char* pic_in;
   	unsigned char* pic_grey;
	int imgWidth;
	int imgHeight; 
	sem_t *sema;
};

unsigned char RGB2grey(int w, int h, unsigned char* pic_in, int imgWidth)
{
	int tmp = (
		pic_in[3 * (h*imgWidth + w) + MYRED] +
		pic_in[3 * (h*imgWidth + w) + MYGREEN] +
		pic_in[3 * (h*imgWidth + w) + MYBLUE] )/3;

	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

unsigned char SobelFilter(int w, int h, int* filter_G, unsigned char* pic_grey, int imgWidth, int imgHeight)
{
	int tmp = 0;
	int a, b;
	int ws = (int)sqrt((float)FILTER_SIZE);
	for (int j = 0; j<ws; j++)
	for (int i = 0; i<ws; i++)
	{
		a = w + i - (ws / 2);
		b = h + j - (ws / 2);

		// detect for borders of the image
		if (a<0 || b<0 || a>=imgWidth || b>=imgHeight) continue;

		tmp += filter_G[j*ws + i] * pic_grey[b*imgWidth + a];
	};
	//tmp /= FILTER_SCALE;
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}


void *Gradient_cal(void *arg) {
	thread_data* param = (thread_data*) arg;
	int height_from = param->i;
	int height_to = param->j;
	int* filter = param->filter;
	int* pic_tmp = param->pic_tmp;
	unsigned char* pic_grey = param->pic_grey;
	int imgWidth = param->imgWidth;
	int imgHeight = param->imgHeight;
	//cout << "hello2\n";
	pthread_mutex_lock(param->mutex);
	for (int j = height_from; j<height_to; j++) {
		for (int i = 0; i< imgWidth; i++) {
			int temp = SobelFilter(i, j, filter, pic_grey, imgWidth, imgHeight);
			pic_tmp[j*imgWidth + i] += temp*temp;
		}
	}
	pthread_mutex_unlock(param->mutex);

	pthread_exit(NULL);
}

void *thr_SobelBlur(void *arg) {
	file_exchange* info = (file_exchange*) arg;
	int id = info->id;
	int imgWidth = info->imgWidth;
	int imgHeight = info->imgHeight;
	int* pic_tmp = info->pic_tmp;
	unsigned char* pic_grey = info->pic_grey;

  	int height_from = id * imgHeight / NUM_THREADS_SMALL;
  	int height_to = (id+1) * imgHeight / NUM_THREADS_SMALL;
  	pthread_mutex_t mutex;
  	pthread_mutex_init(&mutex, NULL);
  	pthread_t thread_x, thread_y;

	thread_data data_x = {height_from,height_to,filter_Gx,&mutex,pic_tmp,pic_grey,imgWidth,imgHeight};
	thread_data data_y = {height_from,height_to,filter_Gy,&mutex,pic_tmp,pic_grey,imgWidth,imgHeight};
	pthread_create(&thread_x, NULL, Gradient_cal, (void* )&data_x);
	pthread_create(&thread_y, NULL, Gradient_cal, (void* )&data_y);
	pthread_join(thread_x, NULL);
	pthread_join(thread_y, NULL);
  	//cout << "hello1\n";

	pthread_exit(NULL);
}

void *thr_RGB2Grey(void* arg){
	convert_data* para = (convert_data*)arg;
	unsigned char* pic_in = para->pic_in;
	unsigned char* pic_grey = para->pic_grey;
	int imgWidth = para->imgWidth;
	int imgHeight = para->imgHeight;

	for (int j = 0; j<imgHeight; j++) {
		for (int i = 0; i<imgWidth; i++){
			pic_grey[j*imgWidth + i] = RGB2grey(i, j, pic_in, imgWidth);
		}
	}
	sem_post(para->sema);
	pthread_exit(NULL);
}

void* parallel_five(void *arg) {
	int k = (long) arg;
	BmpReader* bmpReader = new BmpReader;
	unsigned char *pic_in, *pic_grey, *pic_final;
	int *pic_tmp;
	int imgWidth, imgHeight;
	int rc;
	pthread_t temp[NUM_THREADS_SMALL];
	// read input BMP file
	pic_in = bmpReader->ReadBMP(inputfile_name[k], &imgWidth, &imgHeight);
	// allocate space for output image
	pic_grey = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
	pic_tmp = (int*)malloc(imgWidth*imgHeight*sizeof(int));
	//pic_gy = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
	pic_final = (unsigned char*)malloc(3 * imgWidth*imgHeight*sizeof(unsigned char));
	
	//semaphore
	sem_t sema;
	sem_init(&sema, 0, 0);

	convert_data para = {pic_in,pic_grey,imgWidth,imgHeight,&sema};
	pthread_t thread_convert;
	if ((rc = pthread_create(&thread_convert, NULL, thr_RGB2Grey, (void*)&para))) {
    	fprintf(stderr, "convert error: pthread_create, rc: %d\n", rc);
    	pthread_exit(NULL);
    }

    pthread_join(thread_convert, NULL);
    sem_wait(&sema);
	/*for (int j = 0; j<imgHeight; j++) {
		for (int i = 0; i<imgWidth; i++){
			pic_grey[j*imgWidth + i] = RGB2grey(i, j, pic_in,imgWidth);
		}
	}*/
	
	// try to parallel Sobel with N threads 
	file_exchange info[NUM_THREADS_SMALL]; // {i,imgWidth,imgHeight,pic_tmp,pic_grey};
	for (int i = 0; i < NUM_THREADS_SMALL; ++i) {
		info[i] = {i,imgWidth,imgHeight,pic_tmp,pic_grey};
		if ((rc = pthread_create(&temp[i], NULL, thr_SobelBlur, (void*)&info[i]))) {
   		fprintf(stderr, "small thread error: pthread_create, rc: %d\n", rc);
   		pthread_exit(NULL);
   		}
	}

	for (int i = 0; i < NUM_THREADS_SMALL; ++i) {
		pthread_join(temp[i], NULL);
	}
	
	//extend the size form WxHx1 to WxHx3
	for (int j = 0; j<imgHeight; j++) {
		for (int i = 0; i<imgWidth; i++){
			int temp = sqrt(pic_tmp[j*imgWidth + i]);
			//if (temp < 0) temp = 0;
			if (temp > 255) temp = 255;
			pic_final[3 * (j*imgWidth + i) + MYRED] = (unsigned char)temp;
			pic_final[3 * (j*imgWidth + i) + MYGREEN] = (unsigned char)temp;
			pic_final[3 * (j*imgWidth + i) + MYBLUE] = (unsigned char)temp;
		}
	}

	// write output BMP file
	bmpReader->WriteBMP(outputSobel_name[k], imgWidth, imgHeight, pic_final);
	
	//free memory space
	free(pic_final);
	free(pic_tmp);
	free(pic_grey);
	free(pic_in);
	free(bmpReader);
	pthread_exit(NULL);
}


int main()
{
	pthread_t thread[NUM_THREADS];
	int rc;
	// read mask file
	FILE* mask;
	mask = fopen("mask_Sobel.txt", "r");
	fscanf(mask, "%d", &FILTER_SIZE);

	filter_Gx = new int[FILTER_SIZE];
	filter_Gy = new int[FILTER_SIZE];
	for (int i = 0; i<FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_Gx[i]);
	for (int i = 0; i<FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_Gy[i]);
	fclose(mask);

	for (int i = 0; i < NUM_THREADS; ++i) {
		if ((rc = pthread_create(&thread[i], NULL, parallel_five, (void*)i))) {
      		fprintf(stderr, "parallel five error: pthread_create, rc: %d\n", rc);
      		return EXIT_FAILURE;
    	}
	}

	for (int i = 0; i < NUM_THREADS; ++i) {
		pthread_join(thread[i], NULL);
	}

	return 0;

}