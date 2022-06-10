//
// Created by itamar on 02/06/2022.
//

#ifndef EX3_BOUNDEDQUEUE_H
#define EX3_BOUNDEDQUEUE_H
// A structure to represent a queue

pthread_mutex_t* producerLock;
sem_t* producerFull;
sem_t* producerEmpty;

pthread_mutex_t screenMngLock;
sem_t screenMngFull;
sem_t screenMngEmpty;

int ret;

typedef struct bQueue {
    int front, rear, size;
    unsigned capacity;
    char** array;
} bQueue;

bQueue* createQueue(unsigned capacity);
int isFull(bQueue* queue);
int isEmpty(bQueue* queue);
void enqueue(bQueue* queue, const char* item);
char* dequeue(bQueue* queue);
int front(bQueue* queue);
int rear(bQueue* queue);
void delqueue(bQueue* queue);

#endif //EX3_BOUNDEDQUEUE_H
