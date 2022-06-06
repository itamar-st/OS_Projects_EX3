// C program for array implementation of queue
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "boundedQueue.h"


pthread_mutex_t lock;
sem_t full;
sem_t empty;
int ret;
// function to create a queue
// of given capacity.
// It initializes size of queue as 0
bQueue* createQueue(unsigned capacity)
{
    bQueue* queue = (struct bQueue*)malloc(sizeof(bQueue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;

    // This is important, see the enqueue
    queue->rear = capacity - 1;
    queue->array = (char**)malloc(queue->capacity * sizeof(char*));
    if((ret = sem_init(&full,0,0)) != 0 ){
        perror("sem_init full");
    }
    if((ret = sem_init(&empty,0,capacity)) != 0 ){
        perror("sem_init empty");
    }
    return queue;
}

// bQueue is full when size becomes
// equal to the capacity
int isFull(struct bQueue* queue)
{
    return (queue->size == queue->capacity);
}

// bQueue is empty when size is 0
int isEmpty(struct bQueue* queue)
{
    return (queue->size == 0);
}

// Function to add an item to the queue.
// It changes rear and size
void enqueue(bQueue* queue, char* item)
{
    if((ret = sem_wait(&empty)) != 0){
        perror("sem_trywait empty");
    }
    pthread_mutex_lock(&lock);
    if (isFull(queue)) {
        return;
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    printf("%s enqueued to queue\n", item);

    pthread_mutex_unlock(&lock);

    if((ret = sem_post(&full)) != 0){
        perror("sem_post full");
    }
}

// Function to remove an item from queue.
// It changes front and size
const char* dequeue(struct bQueue* queue)
{
    if((ret = sem_wait(&full)) != 0){
        perror("sem_wait full");
    }
    pthread_mutex_lock(&lock);
    const char* item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    printf("%s removed from queue\n", item);
    pthread_mutex_unlock(&lock);
    if((ret = sem_post(&empty)) != 0){
        perror("sem_post empty");
    }
    return item;

}

// Function to get front of queue
int front(struct bQueue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}

// Function to get rear of queue
int rear(struct bQueue* queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}

void delqueue(struct bQueue* queue){
    free(queue->array);
    pthread_mutex_destroy(&lock);
    if((ret = sem_destroy(&full)) != 0){
        perror("sem_destroy full");
    }
    if((ret = sem_destroy(&empty)) != 0){
        perror("sem_destroy empty");
    }
}
