#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    pid_t pid1, pid2, pid3, pid4, pid5;
    
    printf("Main Process ID:%d\n\n", getpid());
    pid1 = fork();

    if (pid1 < 0){
        fprintf(stderr, "Fork 1 failed" );
        return 1;
    }   
    else if (pid1 == 0){    // first child
        pid2 = fork();
        if (pid2 < 0){
            fprintf(stderr, "Fork 2 failed" );
            return 1;
        }
        else if (pid2 == 0){
            int temp = usleep(1);
            pid4 = fork();
            if (pid4 < 0){
                fprintf(stderr, "Fork 4 failed" );
                return 1;
            }
            else if (pid4 == 0){
                exit(1);
            }
            else{
                
                printf("Fork %d. I'm the child %d, my parent is %d.\n",3,pid4, getpid());
                //printf("Fork %d 4. I'm the child %d, my parent is %d.\n",3,pid4, getpid());
                wait(NULL);
            }
            exit(1);
        }
        else{
            printf("Fork %d. I'm the child %d, my parent is %d.\n",2,pid2, getpid());
            //printf("Fork %d 2. I'm the child %d, my parent is %d.\n",2,pid2, getpid());
            pid3 = fork();
            if (pid3 < 0){
                fprintf(stderr, "Fork 3 failed" );
                return 1;
            }
            else if (pid3 == 0){
                int temp = usleep(2);
                pid5 = fork();
                if (pid5 < 0){
                    fprintf(stderr, "Fork 5 failed" );
                    return 1;
                }
                else if (pid5 == 0){
                    exit(1);
                }
                else{
                    printf("Fork %d. I'm the child %d, my parent is %d.\n",3,pid5, getpid());
                    //printf("Fork %d 5. I'm the child %d, my parent is %d.\n",3,pid5, getpid());
                    wait(NULL);
                }
                exit(1);
            }
            else{
                printf("Fork %d. I'm the child %d, my parent is %d.\n",2,pid3, getpid());
                //printf("Fork %d 3. I'm the child %d, my parent is %d.\n",2,pid3, getpid());
                wait(NULL);
            }
            wait(NULL);
        }
        exit(1);
    }
    else{    // main process
        printf("Fork %d. I'm the child %d, my parent is %d.\n",1,pid1, getpid());
        //printf("Fork %d 1. I'm the child %d, my parent is %d.\n",1,pid1, getpid());
        wait(NULL);
    }


    return 0;
}
