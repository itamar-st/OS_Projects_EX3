//
// Created by itamar on 02/06/2022.
//

#ifndef EX3_UNBOUNDEDQUEUE_H
#define EX3_UNBOUNDEDQUEUE_H
// A structure to represent a queue

pthread_mutex_t editorLock[3];
sem_t editorFull[3];
sem_t editorEmpty[3];
int edRet;

typedef struct ubQueue {
    int front, rear, size;
    unsigned capacity;
    char** array;
} ubQueue;
ubQueue* ubqCreateQueue(unsigned initCapacity);
int ubqIsFull( ubQueue* queue);
int ubqisEmpty( ubQueue* queue);
void ubqEnqueue( ubQueue* queue, const char* item);
char* ubqDequeue( ubQueue* queue);
int ubqFront( ubQueue* queue);
int ubqRear( ubQueue* queue);
void ubqDelqueue( ubQueue* queue);

#endif //EX3_UNBOUNDEDQUEUE_H
