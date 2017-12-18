// Student ID	: 0310120
// Name      	:
// Date      	: 2017.11.03

#include "bmpReader.h"
#include "bmpReader.cpp"
#include <stdio.h>
#include <iostream>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
using namespace std;

#define MYRED	2
#define MYGREEN 1
#define MYBLUE	0
#define NUM_THREADS 5
#define NUM_THREADS_SMALL 5

//int imgWidth, imgHeight;
int FILTER_SIZE;
int FILTER_SCALE;
int *filter_G;


const char *inputfile_name[5] = {
	"input1.bmp",
	"input2.bmp",
	"input3.bmp",
	"input4.bmp",
	"input5.bmp"
};
const char *outputBlur_name[5] = {
	"Blur1.bmp",
	"Blur2.bmp",
	"Blur3.bmp",
	"Blur4.bmp",
	"Blur5.bmp"
};

struct thread_data {
   int  id;
   unsigned char* pic_grey, *pic_blur;
   int imgWidth, imgHeight;
};

struct convert_data {
   	unsigned char* pic_in;
   	unsigned char* pic_grey;
	int imgWidth;
	int imgHeight; 
	sem_t *sema;
};


unsigned char RGB2grey(int w, int h, unsigned char *pic_in, int imgWidth, int imgHeight)
{
	int tmp = (
		pic_in[3 * (h*imgWidth + w) + MYRED] +
		pic_in[3 * (h*imgWidth + w) + MYGREEN] +
		pic_in[3 * (h*imgWidth + w) + MYBLUE] )/3;

	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}

unsigned char GaussianFilter(int w, int h, unsigned char *pic_grey, int imgWidth, int imgHeight)
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
	tmp /= FILTER_SCALE;
	if (tmp < 0) tmp = 0;
	if (tmp > 255) tmp = 255;
	return (unsigned char)tmp;
}


void *thr_GaussianBlur(void *arg) {
	thread_data* param = (thread_data*) arg;
  	int height_from = param->id * param->imgHeight / NUM_THREADS_SMALL;
  	int height_to = (param->id+1) * param->imgHeight / NUM_THREADS_SMALL;

	for (int j = height_from; j<height_to; j++) {
		for (int i = 0; i< param->imgWidth; i+=1){
			param->pic_blur[j*param->imgWidth + i] = GaussianFilter(i, j, param->pic_grey, 
													param->imgWidth, param->imgHeight);
		}
	}

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
			pic_grey[j*imgWidth + i] = RGB2grey(i, j, pic_in, imgWidth, imgHeight);
		}
	}
	sem_post(para->sema);
	pthread_exit(NULL);
}


void* great_work(void *arg) {
	pthread_t temp[NUM_THREADS_SMALL];

	BmpReader* bmpReader = new BmpReader();
	int k = (long) arg;
	int imgWidth, imgHeight, rc;
	unsigned char *pic_in, *pic_grey, *pic_blur, *pic_final;

	//semaphore
	sem_t sema;
	sem_init(&sema, 0, 0);

	pic_in = bmpReader->ReadBMP(inputfile_name[k], &imgWidth, &imgHeight);

	// allocate space for output image
	pic_grey = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
	pic_blur = (unsigned char*)malloc(imgWidth*imgHeight*sizeof(unsigned char));
	pic_final = (unsigned char*)malloc(3 * imgWidth*imgHeight*sizeof(unsigned char));

	convert_data info = {pic_in,pic_grey,imgWidth,imgHeight,&sema};
	pthread_t thread_convert;

	if ((rc = pthread_create(&thread_convert, NULL, thr_RGB2Grey, (void*)&info))) {
      	fprintf(stderr, "convert error: pthread_create, rc: %d\n", rc);
      	pthread_exit(NULL);
    }

    pthread_join(thread_convert, NULL);
    sem_wait(&sema);
	/*for (int j = 0; j<imgHeight; j++) {
		for (int i = 0; i<imgWidth; i++){
			pic_grey[j*imgWidth + i] = RGB2grey(i, j, pic_in, imgWidth, imgHeight);
		}
	}*/

	thread_data data[NUM_THREADS_SMALL]; //{i, pic_grey, pic_blur, imgWidth, imgHeight};

	for (int i = 0; i < NUM_THREADS_SMALL; ++i) {
		data[i].id = i;
		data[i].pic_grey = pic_grey;
		data[i].pic_blur = pic_blur;
		data[i].imgWidth = imgWidth;
		data[i].imgHeight = imgHeight;
		if ((rc = pthread_create(&temp[i], NULL, thr_GaussianBlur, (void*)&data[i]))) {
      		fprintf(stderr, "convolve error: pthread_create, rc: %d\n", rc);
      		pthread_exit(NULL);
    		}
	}

	for (int i = 0; i < NUM_THREADS_SMALL; ++i) {
		pthread_join(temp[i], NULL);
	}
	//extend the size form WxHx1 to WxHx3
	for (int j = 0; j<imgHeight; j++) {
		for (int i = 0; i<imgWidth; i++){
			pic_final[3 * (j*imgWidth + i) + MYRED] = pic_blur[j*imgWidth + i];
			pic_final[3 * (j*imgWidth + i) + MYGREEN] = pic_blur[j*imgWidth + i];
			pic_final[3 * (j*imgWidth + i) + MYBLUE] = pic_blur[j*imgWidth + i];
		}
	}

	// write output BMP file
	bmpReader->WriteBMP(outputBlur_name[k], imgWidth, imgHeight, pic_final);

	//free memory space
	 free(pic_in);
	 free(pic_grey);
	 free(pic_blur);
	 free(pic_final);

	pthread_exit(NULL);
}

int main()
{	
    
	pthread_t thread[NUM_THREADS];
	int rc;
	// read mask file
	FILE* mask;
	mask = fopen("mask_Gaussian.txt", "r");
	fscanf(mask, "%d", &FILTER_SIZE);
	fscanf(mask, "%d", &FILTER_SCALE);

	filter_G = new int[FILTER_SIZE];
	for (int i = 0; i<FILTER_SIZE; i++)
		fscanf(mask, "%d", &filter_G[i]);
	fclose(mask);

	for (int i = 0; i < NUM_THREADS; ++i) {
		if ((rc = pthread_create(&thread[i], NULL, great_work, (void*)i))) {
      		fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
      		return EXIT_FAILURE;
    	}
	}

	for (int i = 0; i < NUM_THREADS; ++i) {
		pthread_join(thread[i], NULL);
	}

	return 0;
}