#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

int cores;
int fd[5][2];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


void * doWork(void * arg){

	int n = 0;
	char letter; 

	pthread_mutex_lock(&lock);

	while(1){

		if(read(fd[n][0], &letter, sizeof(char))){

			if(letter == '\0'){
				break;
			}

			printf("%c\n", letter);
	
		}
	}

	


	pthread_mutex_unlock(&lock);
}

int main(int argc, char * argv[]){

	int shiftList[1000] = {};
	int i = 0;
	char letterString[1000] = {};

	printf("%s\n", "Enter your string: ");

	fgets(letterString, 1000, stdin);

	letterString[strlen(letterString) -1] = '\0';

	cores = argc - 1;

	pthread_t tid[1];

	for(int i = 0; i < argc - 1; i++){

		pthread_create(&tid[i], NULL, &doWork, NULL);
	}

	for(int i = 0; i < cores + 1; i++){

		pipe(fd[i]);
	}
	for(int i = 0; i < strlen(letterString); i++){

		write(fd[i][1], &letterString[i], sizeof(int));
	}
	close(fd[0][1]);

	pthread_join(tid[0], NULL);
	/**
	Where fds[i][0] represents read end and fds[i][1] represents write end.

	*/


}
