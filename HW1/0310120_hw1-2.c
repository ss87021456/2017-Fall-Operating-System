#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>

void detachAndReleaseSharedMemory(unsigned int* shm, int shmid);
unsigned int OneProcess(unsigned int* A, unsigned int* B, unsigned int* C,int n);
unsigned int FourProcess(unsigned int* A, unsigned int* B, unsigned int* C,int n);

int main() {
	int n, sec, usec;
	struct timeval start, end;
	int shmid_A, shmid_B, shmid_C;
	key_t key_A, key_B, key_C;

	unsigned int *shm_C, *shm_A, *shm_B;
		
	key_C = 1205;	
	key_A = 105;	
	key_B = 607;
	
	printf("Input the matrix dimension: ");
	scanf("%d", &n);
	
	// create shared memory for matrix A, B, C
    if((shmid_A = shmget(key_A, sizeof(unsigned int) * n * n, IPC_CREAT | 0666)) < 0) {
        perror("shmget: shmget_A failed");
        exit(1);
    	}
    if((shmid_B = shmget(key_B, sizeof(unsigned int) * n * n, IPC_CREAT | 0666)) < 0) {
        perror("shmget: shmget_B failed");
        exit(1);
    	}
    if((shmid_C = shmget(key_C, sizeof(unsigned int) * n * n, IPC_CREAT | 0666)) < 0) {
        perror("shmget: shmget_C failed");
        exit(1);
    	}
	
	// attach ptr to shared memory 
	if((shm_C = shmat(shmid_C, NULL, 0)) == (unsigned int*)-1) {
        perror("shmat_C: attach error");
        exit(1);
    	}
	if((shm_A = shmat(shmid_A, NULL, 0)) == (unsigned int*)-1) {
        perror("shmat_A: attach error");
        exit(1);
    	}
	if((shm_B = shmat(shmid_B, NULL, 0)) == (unsigned int*)-1) {
        perror("shmat_B: attach error");
        exit(1);
    	}

    //initialize share memory A and B
    for (int i = 0; i < n*n ; ++i){
    	shm_A[i] = i;
    	shm_B[i] = i;
    }

    // perform one process
    unsigned int sum = 0;
    gettimeofday(&start, 0);
    sum = OneProcess(shm_A ,shm_B ,shm_C ,n);
    gettimeofday(&end, 0);
    sec = end.tv_sec - start.tv_sec;
    usec = end.tv_usec - start.tv_usec;
    printf("1-process, checksum = %d\n",sum);
    printf("elapsed %f  s\n", sec + (usec/1000000.0));

    for (int i = 0; i < n*n ; ++i){
    	shm_C[i] = 0;
    }
    // perform four process
    gettimeofday(&start, 0);
    sum = FourProcess(shm_A ,shm_B ,shm_C ,n);
    gettimeofday(&end, 0);
    sec = end.tv_sec - start.tv_sec;
    usec = end.tv_usec - start.tv_usec;
    printf("4-process, checksum = %d\n",sum);
    printf("elapsed %f  s\n", sec + (usec/1000000.0));


    // detach and release shared mem
    detachAndReleaseSharedMemory(shm_A,shmid_A);
    detachAndReleaseSharedMemory(shm_B,shmid_B);
    detachAndReleaseSharedMemory(shm_C,shmid_C);

    return 0;
}

void detachAndReleaseSharedMemory(unsigned int* shm, int shmid){
 	if (shmdt(shm) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }
    if (shmctl(shmid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "shmctl(IPC_RMID) failed\n");
        exit(1);
    }
}


unsigned int OneProcess(unsigned int* A, unsigned int* B, unsigned int* C,int n){
    int i, j ,k;
    unsigned int sum = 0;
    for (int i = 0 ; i < n ; ++i){
        for (int j = 0 ; j < n ; ++j){
            C[i*n + j] = 0;
            for (int k = 0 ; k < n ; ++k){
                C[i*n + j] += (A[i*n + k] * B[k*n +j]);
            }
            sum += C[i*n + j];
        }
    }
    return sum;
}

unsigned int FourProcess(unsigned int* A, unsigned int* B, unsigned int* C,int n){

    int i, j, k;
    unsigned int sum = 0;

    // fork here
    pid_t pid, pid2, pid3;
    pid = fork();

    if(pid <0) {
        fprintf(stderr, "Fork Failed");
        exit(-1);
    }
    else if(pid == 0){
        // fork here
        pid2 = fork();

        if(pid2 <0) {
            // if error, fork failed
            fprintf(stderr, "Fork Failed");
            exit(-1);
        }
        else if(pid2 == 0){
            // child, and calc the first part, i=0 ~ n/2
            for( i=0 ; i<n/2 ; i++ ){
                for( j=0 ; j<n/2 ; j++ ){
                    C[i*n + j] = 0;
                    for( k=0 ; k<n ; k++ ){
                        C[i*n + j] += A[i*n + k] * B[k*n + j];
                    }
                }
            }
            exit(0);
        }
        else {
            // parent, i=n/2 ~ n
            for( i=n/2 ; i<n ; i++ ){
                for( j=0 ; j<n/2 ; j++ ){
                    C[i*n + j] = 0;
                    for( k=0 ; k<n ; k++ ){
                        C[i*n + j] += A[i*n + k] * B[k*n + j];
                    }
                }
            }
            wait(NULL);
        }
        exit(0);
    }
    else {
        // fork here
        pid3 = fork();

        if(pid3 <0) {
            // if error, fork failed
            fprintf(stderr, "Fork Failed");
            exit(-1);
        }
        else if(pid3 == 0){
            // child, and calc the first part, i=0 ~ n/2
            for( i=0 ; i<n/2 ; i++ ){
                for( j=n/2 ; j<n ; j++ ){
                    C[i*n + j] = 0;
                    for( k=0 ; k<n ; k++ ){
                        C[i*n + j] += A[i*n + k] * B[k*n + j];
                    }
                }
            }
           // detachSharedMemory();
            exit(0);
        }
        else {
            // parent, i=n/2 ~ n
            for( i=n/2 ; i<n ; i++ ){
                for( j=n/2 ; j<n ; j++ ){
                    C[i*n + j] = 0;
                    for( k=0 ; k<n ; k++ ){
                        C[i*n + j] += A[i*n + k] * B[k*n + j];
                    }
                }
            }
            // if parent
            wait(NULL);
            wait(NULL);
            // sum up here
            for( i=0 ; i<n*n ; i++ ) 
            	sum += C[i];
        }
    }
    return sum;
}
