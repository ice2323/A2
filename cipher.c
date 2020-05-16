#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#define SIZE 15
int to_child_fd[2];
int to_parent_fd[2];
    char message[SIZE];


void * doWork(void * arg){

	char message[SIZE];
	int result;
	int shiftLength = *((int *) arg);
	int ch;
	char sentences[SIZE];

	read(to_parent_fd[0], message, SIZE);

	int msgSize = strlen(message);

	for(int i = 0; i < msgSize; i++){

		ch = message[i];

		if(ch >= 'a' && ch <= 'z'){
			ch = ch + shiftLength;
			if(ch > 'z'){
				ch = ch - 'z' + 'a' - 1;
			}else if(ch < 'a'){
				ch = ch + 26;
			}
		}else if(ch >= 'A' && ch <= 'Z'){
				ch = ch + shiftLength;
			if(ch > 'Z'){
				ch = ch - 'Z' + 'A' - 1;
			}else if(ch < 'A'){
				ch = ch + 26;
			}
		}		
		sentences[i] = ch;
	}
	for(int i =  0; i < msgSize; i++){
		printf("%c", sentences[i]);
	}
	printf("\n");
	write(to_parent_fd[1], sentences,  100);
}

int main(int argc, char * argv[]){
    
    int shiftList[] = {};
    int value;
    int counter = 0;
    int cores = 0;

    for(int i = 1; i < argc; i++){

    	shiftList[counter] = atoi(argv[i]);
    	counter++;
    }
    int *currentShift = malloc(sizeof(*currentShift));
    *currentShift = shiftList[0];

	pipe(to_child_fd);
    pipe(to_parent_fd);
    if (fork() == 0) {
    int length;

    close(to_child_fd[1]);  /* child closes write side of child  pipe */
    close(to_parent_fd[0]); /* child closes read  side of parent pipe */
    length = read(to_child_fd[0], message, SIZE);
    
    write(to_parent_fd[1], message, strlen(message) + 1);
    close(to_parent_fd[1]);
    } else {

    char phrase[SIZE];
    char message[SIZE];
    printf("please type a sentence : ");
    scanf("%s", phrase);
    close(to_parent_fd[1]); /* parent closes write side of parent pipe */
    close(to_child_fd[0]);  /* parent closes read  side of child  pipe */
    write(to_child_fd[1], phrase, strlen(phrase) + 1);
    //close(to_child_fd[1]);
    //read(to_parent_fd[0], message, SIZE);

    pthread_t tid[1];


    pthread_create(&tid[0], NULL, &doWork, currentShift);
    //sleep(10);
    pthread_join(tid[0], NULL);

    //pthread_create(&tid[1], NULL, &doWork, NULL);



    //pthread_join(tid[1], NULL);

    }
    return 0;
}
