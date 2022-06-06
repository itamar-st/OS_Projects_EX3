#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "boundedQueue.h"
#include "unboundedQueue.h"

#include<stdio.h>
#include <stdlib.h>

bQueue** producerQueue;
pthread_t* producerThread;

pthread_t dispatcherThread;

ubQueue* editorsQueue[3];
pthread_t editorSPORTSThread;
pthread_t editorNEWSThread;
pthread_t editorWEATHERThread;

typedef struct producerThreadArgs{
    int producerNum;
    int num_of_products;
    int buff_size;
} producerThreadArgs;

void* producer(void* args){
    producerThreadArgs a = *(producerThreadArgs *) args;
    producerQueue[a.producerNum -1] = createQueue(a.buff_size);
    char* newstype[3] = {" SPORTS ", " NEWS ", " WEATHER "};
    int newsCount[3];
    int numOfProd = a.num_of_products;
    int i;
    for (i = 0; i< numOfProd; i++){
        char* currentNewsType = newstype[i%3];
        char newsToSend[30] = "producer ";
        char produNum[2];
        char newsNum[2];
        sprintf(produNum, "%d", a.producerNum);
        sprintf(newsNum, "%d", newsCount[i%3]);
        strcat(newsToSend, produNum);
        strcat(newsToSend, currentNewsType);
        strcat(newsToSend, newsNum);
        enqueue(producerQueue[a.producerNum - 1], newsToSend);
        newsCount[i%3]++;
    }
    enqueue(producerQueue[a.producerNum - 1], "DONE");
}

void* dispatcher(void* args){
    int numOfProducers = (int) args;
    printf("enter to dispatcher\n");
//    char* newstype[3] = {" SPORTS ", " NEWS ", " WEATHER "};
//    int newsCount[3];
    int producerQueueDone[numOfProducers];
    int n;
    for(n=0; n< numOfProducers; n++){
        producerQueueDone[n] = 0;
    }

    const char* currentNews;
    while (1){
        currentNews = dequeue(producerQueue[0]);
        char item[32] = "";
        strcpy(item, currentNews);
        const char s[2] = " ";
        char* token;
        /* get the third token */
        strtok(item, s);
        strtok(NULL, s);
//        strtok(NULL, s);
        token = strtok(NULL, s);
        if(strcmp(token, "SPORTS") == 0){
            printf("dispatcher SPORTS\n");
        }
        else if(strcmp(token, "NEWS") == 0){
            printf("dispatcher NEWS\n");
        }
        else if(strcmp(token, "WEATHER") == 0){
            printf("dispatcher WEATHER\n");
        }
        else if(strcmp(token, "DONE") == 0){
            printf("dispatcher DONE\n");
            break;
        }
    }
}
int main(int argc, char *argv[])
{
    FILE* confFD;
    if((confFD=fopen(argv[1],"r"))== NULL){
        perror("open conf");
        exit(-1);
    }
    int ch = 0;
    int numOfLines = 0;
    while(!feof(confFD))
    {
        ch = fgetc(confFD);
        if(ch == '\n')
        {
            numOfLines++;
        }
    }
    fclose(confFD);

    numOfLines = numOfLines/4;
    producerThread = (pthread_t*) malloc(numOfLines * sizeof(pthread_t));
    producerQueue = (bQueue**) malloc(numOfLines * sizeof(bQueue*));

    if((confFD=fopen(argv[1],"r"))== NULL){
        perror("open conf");
        exit(-1);
    }
    char* line = NULL;
    size_t lineSize = 32;
    int j;
    producerThreadArgs producerArgs;
    for(j=0; j<numOfLines; j++){
        getline(&line, &lineSize, confFD);
        producerArgs.producerNum = atoi(line);
        getline(&line, &lineSize, confFD);
        producerArgs.num_of_products = atoi(line);
        getline(&line, &lineSize, confFD);
        producerArgs.buff_size = atoi(line);
        getline(&line, &lineSize, confFD);
        pthread_create(&producerThread[producerArgs.producerNum - 1], NULL, producer, &producerArgs );
        printf("thread created\n");
    }

    pthread_create(&dispatcherThread, NULL, dispatcher, NULL );
    void* retVal;
//    sleep(5);
    int q;
    for(q=0; q<numOfLines; q++){
        if(pthread_join(producerThread[q],&retVal) != 0){
            perror("pthread_join");
        }
    }
    if(pthread_join(dispatcherThread,&retVal) != 0){
        perror("pthread_join");
    }

    getline(&line, &lineSize, confFD);

    fclose(confFD);
    free(producerThread);
    free(producerQueue);
    return 0;
}