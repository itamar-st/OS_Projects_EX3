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

        if((ret = sem_wait(&empty)) != 0){
            perror("sem_trywait empty");
        }
        pthread_mutex_lock(&lock);
        enqueue(producerQueue[a.producerNum - 1], finalNewsToSend);
        pthread_mutex_unlock(&lock);

        if((ret = sem_post(&full)) != 0){
            perror("sem_post full");
        }

        newsCount[i%3]++;
    }
    if((ret = sem_wait(&empty)) != 0){
        perror("sem_trywait empty");
    }
    pthread_mutex_lock(&lock);
    enqueue(producerQueue[a.producerNum - 1], "DONE");
    printf("producer - DONE\n");
    pthread_mutex_unlock(&lock);

    if((ret = sem_post(&full)) != 0){
        perror("sem_post full");
    }
}

void* dispatcher(void* args){
    printf("enter to dispatcher\n");
    const char* currentNews;
    while (1){
//        int val1;
//        sem_getvalue(&full,&val1);
//        printf("-----------------dispatcher - val of full:   %d\n", val1);
        if((ret = sem_wait(&full)) != 0){
            perror("sem_wait full");
        }
        pthread_mutex_lock(&lock);
        currentNews = dequeue(producerQueue[0]);   // todo: change 0 to producerQueue[a.producerNum - 1]
        pthread_mutex_unlock(&lock);

        if((ret = sem_post(&empty)) != 0){
            perror("sem_post empty");
        }

        char item[32] = "";
        strcpy(item, currentNews);
        if(strcmp(item, "DONE") == 0){
//            int val;
//            sem_getvalue(&editorEmpty[0],&val);
//            printf("-----------------val:   %d", val);
            if((ret = sem_wait(&editorEmpty[0])) != 0){
                perror("sem_trywait empty");
            }
            pthread_mutex_lock(&editorLock[0]);
            ubqEnqueue(editorsQueue[0], "DONE");
            pthread_mutex_unlock(&editorLock[0]);

            if((ret = sem_post(&editorFull[0])) != 0){
                perror("sem_post full");
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
        if(strcmp(token, "SPORTS") == 0){
            if((ret = sem_wait(&editorEmpty[0])) != 0){
                perror("sem_trywait empty");
            }
            pthread_mutex_lock(&editorLock[0]);
            ubqEnqueue(editorsQueue[0], currentNews);
            pthread_mutex_unlock(&editorLock[0]);

            if((ret = sem_post(&editorFull[0])) != 0){
                perror("sem_post full");
            }
            printf("dispatcher SPORTS\n");
        }
//        else if(strcmp(token, "NEWS") == 0){
//            if((ret = sem_wait(&editorEmpty[1])) != 0){
//                perror("sem_trywait empty");
//            }
//            pthread_mutex_lock(&editorLock[1]);
//            ubqEnqueue(editorsQueue[1], currentNews);
//            pthread_mutex_unlock(&editorLock[1]);
//
//            if((ret = sem_post(&editorFull[1])) != 0){
//                perror("sem_post full");
//            }            printf("dispatcher NEWS\n");
//        }
//        else if(strcmp(token, "WEATHER") == 0){
//            if((ret = sem_wait(&editorEmpty[2])) != 0){
//                perror("sem_trywait empty");
//            }
//            pthread_mutex_lock(&editorLock[2]);
//            ubqEnqueue(editorsQueue[2], currentNews);
//            pthread_mutex_unlock(&editorLock[2]);
//
//            if((ret = sem_post(&editorFull[2])) != 0){
//                perror("sem_post full");
//            }            printf("dispatcher WEATHER\n");
//        }
    }
//    ubqEnqueue(sportsQueue, "DONE");
//    ubqEnqueue(newsQueue,"DONE");
//    ubqEnqueue(weatherQueue, "DONE");

}

void* editor(void* args) {
    editorThreadArgs threadArgs = *(editorThreadArgs *) args;
    printf("enter to editor %d\n", threadArgs.editorNum);

    char *currentMsg = "";
    while (strcmp(currentMsg, "DONE") != 0) {
//        int val;
//        sem_getvalue(&editorFull[0],&val);
//        printf("-----------------editor full val:   %d\n", val);
        if((edRet = sem_wait(&editorFull[threadArgs.editorNum])) != 0){
            perror("sem_wait editorFull");
        }
        pthread_mutex_lock(&lock);
        currentMsg = ubqDequeue(editorsQueue[threadArgs.editorNum]);
        usleep(100000);
        enqueue(screenMngQueue, currentMsg);
        pthread_mutex_unlock(&lock);

        if((edRet = sem_post(&editorEmpty[threadArgs.editorNum])) != 0){
            perror("sem_post editorEmpty");
        }
//        printf("from editor %d: %s\n", threadArgs.editorNum, currentMsg);
    }
    if((edRet = sem_wait(&editorFull[threadArgs.editorNum])) != 0){
        perror("sem_wait editorFull");
    }
    pthread_mutex_lock(&lock);
    currentMsg = ubqDequeue(editorsQueue[threadArgs.editorNum]);
    enqueue(screenMngQueue, currentMsg);
    pthread_mutex_unlock(&lock);

    if((edRet = sem_post(&editorEmpty[threadArgs.editorNum])) != 0){
        perror("sem_post editorEmpty");
    }
    printf("CO - EDITOR %d - DONE\n", threadArgs.editorNum);
}

void* screenManager(void* args) {
    char* queueSize = (char *) args;
    printf("enter to screen Manager\n");

    char *currentMsg = "";
    while (strcmp(currentMsg, "DONE") != 0) {
//        int val;
//        sem_getvalue(&editorFull[0],&val);
//        printf("-----------------editor full val:   %d\n", val);
        if((edRet = sem_wait(&screenMngFull) != 0){
            perror("sem_wait editorFull");
        }
        pthread_mutex_lock(&lock);
        currentMsg = dequeue(screenMngQueue);
        usleep(100000);
        pthread_mutex_unlock(&lock);

        if((edRet = sem_post(&screenMngEmpty) != 0){
            perror("sem_post editorEmpty");
        }
        printf("from editor %d: %s\n", threadArgs.editorNum, currentMsg);
    }

    printf("SCREEN MANAGER - DONE\n");
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
        producerQueue[producerArgs.producerNum -1] = createQueue(producerArgs.buff_size);
        if((ret = sem_init(&full,0,0)) != 0 ){
            perror("sem_init full");
        }
        if((ret = sem_init(&empty,0, producerArgs.buff_size)) != 0 ){
            perror("sem_init empty");
        }
        if((ret = pthread_mutex_init(&lock,PTHREAD_MUTEX_DEFAULT)) != 0){
            perror("pthread_mutex_init");
        }
        pthread_create(&producerThread[producerArgs.producerNum - 1], NULL, producer, &producerArgs );
        printf("thread created\n");
    }

    getline(&line, &lineSize, confFD);
    fclose(confFD);

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
    editorThreadArgs editorArgs; //todo: change
    for (t = 0; t <= 0; ++t) {
        editorArgs.editorNum = t;
        editorArgs.queueInitSize = EDITOR_INIT_QUEUE_SIZE;
        editorsQueue[editorArgs.editorNum] = ubqCreateQueue(editorArgs.queueInitSize);
        if ((edRet = sem_init(&editorFull[editorArgs.editorNum], 0, 0)) != 0) {
            perror("sem_init full");
        }
        if ((edRet = sem_init(&editorEmpty[editorArgs.editorNum], 0, editorArgs.queueInitSize)) != 0) {
            perror("sem_init empty");
        }
        if ((edRet = pthread_mutex_init(&editorLock[editorArgs.editorNum], PTHREAD_MUTEX_DEFAULT)) != 0) {
            perror("pthread_mutex_init");
        }
        pthread_create(&editorThread[t], NULL, editor, &editorArgs );

    }

    void* retVal;
    usleep(100000);
    pthread_create(&dispatcherThread, NULL, dispatcher, NULL );

//    pthread_create(&screenMngThread, NULL, screenManager, NULL );

    int q;

    for(q=0; q<1; ++q){
        if(pthread_join(editorThread[q],&retVal) != 0){
            perror("pthread_join editorThread");
        }
    }
    for(q=0; q<numOfLines; ++q){
        delqueue(producerQueue[q]);
    }
    if((ret = pthread_mutex_destroy(&lock)) != 0){
        perror("pthread_mutex_destroy ");
    }
    if((ret = sem_destroy(&full)) != 0){
        perror("sem_destroy full");
    }
    if((ret = sem_destroy(&empty)) != 0){
        perror("sem_destroy empty");
    }

    for(q=0; q<1; ++q){
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



    return 0;
}