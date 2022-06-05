#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "boundedQueue.h"

#include<stdio.h>
#include <stdlib.h>

bQueue** producerQueue;
pthread_t* producerThread;

typedef struct producerThreadArgs{
    int producerNum;
    int num_of_products;
    int buff_size;
} producerThreadArgs;

void* producer(void* args){
    producerThreadArgs a = *(producerThreadArgs *) args;
//    printf("from thread : %d,%d\n",a.buff_size, a.num_of_products );
//    bQueue* myQueue = createQueue(a.buff_size);
    producerQueue[a.producerNum -1] = createQueue(a.buff_size);
//    struct bQueue* myQueue = producerQueue[a.producerNum];
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
    delqueue(producerQueue[a.producerNum]);
}
int main(int argc, char *argv[])
{
    FILE* confFD;
    if((confFD=fopen(argv[1],"r"))== NULL)
    {
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

    if((confFD=fopen(argv[1],"r"))== NULL)
    {
        perror("open conf");
        exit(-1);
    }
    char* line = NULL;
    size_t lineSize = 32;
    int j;
    for(j=0; j<numOfLines; j++){
        producerThreadArgs producerArgs;
        getline(&line, &lineSize, confFD);
        producerArgs.producerNum = atoi(line);
        getline(&line, &lineSize, confFD);
        producerArgs.num_of_products = atoi(line);
        getline(&line, &lineSize, confFD);
        producerArgs.buff_size = atoi(line);
        getline(&line, &lineSize, confFD);
        numOfLines--;
        pthread_create(&producerThread[producerArgs.producerNum - 1], NULL, producer, &producerArgs );
        void* retVal;
        printf("asdsdads\n");
        pthread_join(producerThread[producerArgs.producerNum - 1],&retVal);

    }
        getline(&line, &lineSize, confFD);

    fclose(confFD);
//    int threadResult;
//    threadResult = pthread_create(&producerThread, NULL, producer, &producerArgs );
//    void* retVal;
//    sleep(20);
//    pthread_join(producerThread,&retVal);
    free(producerThread);
    free(producerQueue);
    return 0;
}