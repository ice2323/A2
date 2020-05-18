#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#define SIZE 1000
int to_child_fd[2];
int to_parent_fd[2];
int fd[9][2];
int backToMain[2];
int globalI = 0;
int shiftCount = 0;
int shiftList[SIZE] = {};

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void * doWork(void * arg){

    int numShift = *((int*) arg);
	char message[SIZE];
	char phrase[SIZE];
	int result;
	int shiftLength = shiftList[globalI];		
	int ch;
	char sentences[SIZE];
	pipe(backToMain);

	pthread_mutex_lock(&lock);

	read(fd[globalI][0], sentences, SIZE);

	int msgSize = strlen(sentences);

	for(int i = 0; i < msgSize; i++){

		ch = sentences[i];

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
	globalI++;

	if(numShift == globalI){
		write(backToMain[1], sentences, SIZE);
	}else{
		sleep(1);
		write(fd[globalI][1], sentences, SIZE);
	}

	pthread_mutex_unlock(&lock);
}

int main(int argc, char * argv[]){
    
    int value;
    int counter = 0;
    int cores = 0;
    int i;

    for(i = 1; i < argc; i++){

    	cores++;
    	shiftList[counter] = atoi(argv[i]);
    	counter++;
    }
    if(cores > 1000){
    	printf("%s\n", "There is a limit of 1000 ciphers, please try again...");
    	return EXIT_FAILURE;
    }
    int *currentShift = malloc(sizeof(*currentShift));
    *currentShift = shiftList[0];
	pipe(to_child_fd);
    pipe(to_parent_fd);
    int *coreHolder = malloc(sizeof(*coreHolder));
    *coreHolder = cores;
    for(i = 0; i < SIZE; i++){

    	pipe(fd[i]);
    	//note: fd[i][0] is read end fd[i][1] is the write end 
    }
    int j = 0;
    int count = 0;
    int length;
    char phrase[SIZE];
    char message[SIZE];

    printf("%s", "Please type a sentence: ");
    fgets(phrase, 1000, stdin);
    pthread_t tid[cores];

    write(fd[globalI][1], phrase, SIZE); 

    for(i = 0; i < cores; i++){

    	pthread_create(&tid[i], NULL, &doWork, coreHolder);
    	j++;
    	sleep(2);
    }
    for(i = 0; i < cores; i++){
    	pthread_join(tid[i], NULL);
    }
    char encryptedSentence[SIZE];
    read(backToMain[0], encryptedSentence, SIZE);

    printf("%s %s", "Your encrypted sentence is:", encryptedSentence);
    return 0;
}
