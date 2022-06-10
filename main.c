#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include "boundedQueue.h"
#include "unboundedQueue.h"
#include<stdio.h>
#include <stdlib.h>
#define EDITOR_INIT_QUEUE_SIZE 50

bQueue** producerQueue;
pthread_t* producerThread;

pthread_t dispatcherThread;

pthread_t screenMngThread;
bQueue* screenMngQueue;

ubQueue* editorsQueue[3];
pthread_t editorThread[3];




typedef struct producerThreadArgs{
    int producerNum;
    int num_of_products;
    int buff_size;
} producerThreadArgs;
typedef struct editorThreadArgs{
    int editorNum;
    int queueInitSize;
} editorThreadArgs;

void* producer(void* args){
    producerThreadArgs a = *(producerThreadArgs *) args;
    printf("enter to producer %d\n", a.producerNum);
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
        char* finalNewsToSend = (char *) malloc(sizeof(char)*32 + 1);
        strcat(finalNewsToSend, newsToSend);

        if((ret = sem_wait(&producerEmpty[a.producerNum - 1])) != 0){
            perror("sem_trywait empty");
        }
        pthread_mutex_lock(&producerLock[a.producerNum - 1]);
        enqueue(producerQueue[a.producerNum - 1], finalNewsToSend);
        pthread_mutex_unlock(&producerLock[a.producerNum - 1]);

        if((ret = sem_post(&producerFull[a.producerNum - 1])) != 0){
            perror("sem_post full");
        }

        newsCount[i%3]++;
    }
    if((ret = sem_wait(&producerEmpty[a.producerNum] - 1)) != 0){
        perror("sem_trywait empty");
    }
    pthread_mutex_lock(&producerLock[a.producerNum - 1]);
    enqueue(producerQueue[a.producerNum - 1], "DONE");
    printf("producer - DONE\n");
    pthread_mutex_unlock(&producerLock[a.producerNum - 1]);

    if((ret = sem_post(&producerFull[a.producerNum - 1])) != 0){
        perror("sem_post full");
    }
}

void* dispatcher(void* args){
    int numOfProducers = (int) args;
    int producerIndex = 0;
    printf("enter to dispatcher\n");
    const char* currentNews;
    while (1){
//        int val1;
//        sem_getvalue(&full,&val1);
//        printf("-----------------dispatcher - val of full:   %d\n", val1);
        if((ret = sem_wait(&producerFull[producerIndex])) != 0){
            perror("sem_wait full");
        }
        pthread_mutex_lock(&producerLock[producerIndex]);
        currentNews = dequeue(producerQueue[producerIndex]);   // todo: change 0 to producerQueue[a.producerNum - 1]
        pthread_mutex_unlock(&producerLock[producerIndex]);

        if((ret = sem_post(&producerEmpty[producerIndex])) != 0){
            perror("sem_post empty");
        }

        char item[32] = "";
        strcpy(item, currentNews);
        if(strcmp(item, "DONE") == 0){
//            int val;
//            sem_getvalue(&editorEmpty[0],&val);
//            printf("-----------------val:   %d", val);
            int editorNum;
            for (editorNum = 0; editorNum <= 2; ++editorNum) {
                if((ret = sem_wait(&editorEmpty[editorNum])) != 0){
                    perror("sem_trywait empty");
                }
                pthread_mutex_lock(&editorLock[editorNum]);
                ubqEnqueue(editorsQueue[editorNum], "DONE");
                pthread_mutex_unlock(&editorLock[editorNum]);
                if((ret = sem_post(&editorFull[editorNum])) != 0){
                    perror("sem_post full");
                }
            }
            printf("dispatcher - DONE\n");
            break;
        }

        const char s[2] = " ";
        char* token;
        /* get the third token */
        strtok(item, s);
        strtok(NULL, s);
        token = strtok(NULL, s);
        int queueToEnter;
        if(strcmp(token, "SPORTS") == 0){
            queueToEnter = 0;
        }
        else if(strcmp(token, "NEWS") == 0){
            queueToEnter = 1;
        }
        else if(strcmp(token, "WEATHER") == 0){
            queueToEnter = 2;
        }
        if((ret = sem_wait(&editorEmpty[queueToEnter])) != 0){
            perror("sem_trywait empty");
        }
        pthread_mutex_lock(&editorLock[queueToEnter]);
        ubqEnqueue(editorsQueue[queueToEnter], currentNews);
        pthread_mutex_unlock(&editorLock[queueToEnter]);
        if((ret = sem_post(&editorFull[queueToEnter])) != 0){
            perror("sem_post full");
        }

        producerIndex = (producerIndex + 1) % numOfProducers;
        printf("dispatcher enter msg to queue: %d\n", queueToEnter);
    }

}

void* editor(void* args) {
    editorThreadArgs threadArgs = *(editorThreadArgs *) args;
    printf("enter to editor %d\n", threadArgs.editorNum);

    char *currentMsg = "";
    while (1) {
//        int val;
//        sem_getvalue(&editorFull[0],&val);
//        printf("-----------------editor full val:   %d\n", val);
        if((edRet = sem_wait(&editorFull[threadArgs.editorNum])) != 0){
            perror("sem_wait editorFull");
        }
        pthread_mutex_lock(&editorLock[threadArgs.editorNum]);
        currentMsg = ubqDequeue(editorsQueue[threadArgs.editorNum]);
        pthread_mutex_unlock(&editorLock[threadArgs.editorNum]);
        if((edRet = sem_post(&editorEmpty[threadArgs.editorNum])) != 0){
            perror("sem_post editorEmpty");
        }

        if(strcmp(currentMsg, "DONE") == 0){
            if((edRet = sem_wait(&screenMngEmpty)) != 0){
                perror("sem_wait editorFull");
            }
            pthread_mutex_lock(&screenMngLock);
            enqueue(screenMngQueue, currentMsg);
            pthread_mutex_unlock(&screenMngLock);
            if((edRet = sem_post(&screenMngFull)) != 0){
                perror("sem_post editorEmpty");
            }
            break;
        }

        usleep(100000);
        if((edRet = sem_wait(&screenMngEmpty)) != 0){
            perror("sem_wait editorFull");
        }
        pthread_mutex_lock(&screenMngLock);
        enqueue(screenMngQueue, currentMsg);
        pthread_mutex_unlock(&screenMngLock);
        if((edRet = sem_post(&screenMngFull)) != 0){
            perror("sem_post editorEmpty");
        }
//        printf("from editor %d: %s\n", threadArgs.editorNum, currentMsg);
    }

    printf("CO - EDITOR %d - DONE\n", threadArgs.editorNum);
}

void* screenManager(void* args) {
    printf("enter to screen Manager\n");
    int doneCounter = 0;
    char *msgFromQueue = "";
    while (1) {

//        int val;
//        sem_getvalue(&editorFull[0],&val);
//        printf("-----------------editor full val:   %d\n", val);
        if((edRet = sem_wait(&screenMngFull)) != 0){
            perror("sem_wait editorFull");
        }
        pthread_mutex_lock(&screenMngLock);
        msgFromQueue = dequeue(screenMngQueue);
        pthread_mutex_unlock(&screenMngLock);
        if((edRet = sem_post(&screenMngEmpty)) != 0){
            perror("sem_post editorEmpty");
        }

        if(strcmp(msgFromQueue, "DONE") == 0){
            doneCounter++;
            if(doneCounter == 3){ // todo: change to 3
                printf("---------DONE---------\n");
                break;
            }
        } else{
            printf("%s\n", msgFromQueue);
        }
    }
}
int main(int argc, char *argv[])
{
    // read the conf file and check how many lines in there
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

    // every 4 lines = one producer
    numOfLines = numOfLines/4;
    producerThread = (pthread_t*) malloc(numOfLines * sizeof(pthread_t));
    producerQueue = (bQueue**) malloc(numOfLines * sizeof(bQueue*));
    producerLock = (pthread_mutex_t*) malloc(numOfLines * sizeof(pthread_mutex_t));
    producerEmpty = (sem_t*) malloc(numOfLines * sizeof(sem_t));
    producerFull = (sem_t*) malloc(numOfLines * sizeof(sem_t));

    //todo: free memory
    if((confFD=fopen(argv[1],"r"))== NULL){
        perror("open conf");
        exit(-1);
    }
    char* line = NULL;
    size_t lineSize = 32;
    // init producers queue and thread
    int j;
    producerThreadArgs producerArgs[numOfLines];
    for(j=0; j<numOfLines; j++){
        getline(&line, &lineSize, confFD);
        producerArgs[j].producerNum = atoi(line);
        getline(&line, &lineSize, confFD);
        producerArgs[j].num_of_products = atoi(line);
        getline(&line, &lineSize, confFD);
        producerArgs[j].buff_size = atoi(line);
        getline(&line, &lineSize, confFD);
        producerQueue[producerArgs[j].producerNum -1] = createQueue(producerArgs[j].buff_size);
        if((ret = sem_init(&producerFull[producerArgs[j].producerNum -1],0,0)) != 0 ){
            perror("sem_init full");
        }
        if((ret = sem_init(&producerEmpty[producerArgs[j].producerNum -1],0, producerArgs[j].buff_size)) != 0 ){
            perror("sem_init empty");
        }
        if((ret = pthread_mutex_init(&producerLock[producerArgs[j].producerNum -1],PTHREAD_MUTEX_DEFAULT)) != 0){
            perror("pthread_mutex_init");
        }
        pthread_create(&producerThread[producerArgs[j].producerNum - 1], NULL, producer, &producerArgs[j] );
        printf("thread created\n");
    }

    // last line is co editor's shared queue
    getline(&line, &lineSize, confFD);
    fclose(confFD);

    // init screen manager queue and thread
    screenMngQueue = createQueue(atoi(line));
    if ((edRet = sem_init(&screenMngFull, 0, 0)) != 0) {
        perror("sem_init full");
    }
    if ((edRet = sem_init(&screenMngEmpty, 0, atoi(line))) != 0) {
        perror("sem_init empty");
    }
    if ((edRet = pthread_mutex_init(&screenMngLock, PTHREAD_MUTEX_DEFAULT)) != 0) {
        perror("pthread_mutex_init");
    }

    int t;
    editorThreadArgs editorArgs[3]; //todo: change
    for (t = 0; t <= 2; ++t) {
        editorArgs[t].editorNum = t;
        editorArgs[t].queueInitSize = EDITOR_INIT_QUEUE_SIZE;
        editorsQueue[editorArgs[t].editorNum] = ubqCreateQueue(editorArgs[t].queueInitSize);
        if ((edRet = sem_init(&editorFull[editorArgs[t].editorNum], 0, 0)) != 0) {
            perror("sem_init full");
        }
        if ((edRet = sem_init(&editorEmpty[editorArgs[t].editorNum], 0, editorArgs[t].queueInitSize)) != 0) {
            perror("sem_init empty");
        }
        if ((edRet = pthread_mutex_init(&editorLock[editorArgs[t].editorNum], PTHREAD_MUTEX_DEFAULT)) != 0) {
            perror("pthread_mutex_init");
        }
        pthread_create(&editorThread[t], NULL, editor, &editorArgs[t] );

    }

    // init dispatcher thread
    void* retVal;
    pthread_create(&dispatcherThread, NULL, dispatcher, (void *) numOfLines);

    pthread_create(&screenMngThread, NULL, screenManager, NULL );

    int q;

    if(pthread_join(screenMngThread, &retVal) != 0){
        perror("pthread_join screen manager thread");
    }
    // free queue and semaphore for producers
    for(q=0; q<numOfLines; ++q){
        delqueue(producerQueue[q]);
        if((ret = pthread_mutex_destroy(&producerLock[q])) != 0){
            perror("pthread_mutex_destroy ");
        }
        if((ret = sem_destroy(&producerFull[q])) != 0){
            perror("sem_destroy full");
        }
        if((ret = sem_destroy(&producerEmpty[q])) != 0){
            perror("sem_destroy empty");
        }
    }

    // free queue and semaphore for editors
    for(q=0; q<=2; ++q){
        ubqDelqueue(editorsQueue[q]);
        if((edRet = pthread_mutex_destroy(&editorLock[q])) != 0 ){
            perror("pthread_mutex_destroy editorFull");
        }
        if((edRet = sem_destroy(&editorFull[q])) != 0){
            perror("sem_destroy editorFull");
        }
        if((edRet = sem_destroy(&editorEmpty[q])) != 0){
            perror("sem_destroy editorEmpty");
        }
    }
    // free queue and semaphore for screen manager
    delqueue(screenMngQueue);
    if((edRet = pthread_mutex_destroy(&screenMngLock)) != 0 ){
        perror("pthread_mutex_destroy editorFull");
    }
    if((edRet = sem_destroy(&screenMngFull)) != 0){
        perror("sem_destroy editorFull");
    }
    if((edRet = sem_destroy(&screenMngEmpty)) != 0){
        perror("sem_destroy editorEmpty");
    }

    free(producerThread);
    free(producerQueue);
    free(producerLock);
    free(producerEmpty);
    free(producerFull);


    return 0;
}