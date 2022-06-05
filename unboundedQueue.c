// C program for array implementation of queue
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "unboundedQueue.h"



// function to create a queue
// of given capacity.
// It initializes size of queue as 0
struct ubQueue* ubqCreateQueue(unsigned initCapacity)
{
    struct ubQueue* queue = (struct ubQueue*)malloc(sizeof(struct ubQueue));
    queue->capacity = initCapacity;
    queue->front = queue->size = 0;

    // This is important, see the enqueue
    queue->rear = initCapacity - 1;
    queue->array = (int*)malloc(queue->capacity * sizeof(int));
    return queue;
}

// bQueue is full when size becomes
// equal to the capacity
int ubqIsFull(struct ubQueue* queue)
{
    return (queue->size == queue->capacity);
}

// bQueue is empty when size is 0
int ubqisEmpty(struct ubQueue* queue)
{
    return (queue->size == 0);
}

// Function to add an item to the queue.
// It changes rear and size
void ubqEnqueue(struct ubQueue* queue, int item)
{
    if (ubqIsFull(queue)) {
        queue->capacity *= 2;
        queue->array = realloc(queue->array, queue->capacity * sizeof(int));
    }
    queue->rear = (queue->rear + 1)
                  % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    printf("%d enqueued to queue\n", item);
}

// Function to remove an item from queue.
// It changes front and size
int ubqDequeue(struct ubQueue* queue)
{
    if (ubqisEmpty(queue))
        return INT_MIN;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)
                   % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
int ubqFront(struct ubQueue* queue)
{
    if (ubqisEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}

// Function to get rear of queue
int ubqRear(struct ubQueue* queue)
{
    if (ubqisEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}

//void ubqDelqueue(struct bQueue* queue){
//    free(queue->array);
//    pthread_mutex_destroy(&lock);
//    if((ret = sem_destroy(&full)) != 0){
//        perror("sem_destroy full");
//    }
//    if((ret = sem_destroy(&empty)) != 0){
//        perror("sem_destroy empty");
//    }
//}