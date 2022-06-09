// C program for array implementation of queue
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "unboundedQueue.h"



// function to create a queue
// of given capacity.
// It initializes size of queue as 0
ubQueue* ubqCreateQueue(unsigned initCapacity)
{
    printf("EDITOR created\n");
    ubQueue* queue = (ubQueue*)malloc(sizeof(ubQueue));
    queue->capacity = initCapacity;
    queue->front = queue->size = 0;

    // This is important, see the enqueue
    queue->rear = initCapacity - 1;
    queue->array = (char**)malloc(queue->capacity * sizeof(char*));
    return queue;
}

// bQueue is full when size becomes
// equal to the capacity
int ubqIsFull(ubQueue* queue)
{
    return (queue->size == queue->capacity);
}

// bQueue is empty when size is 0
int ubqisEmpty(ubQueue* queue)
{
    return (queue->size == 0);
}

// Function to add an item to the queue.
// It changes rear and size
void ubqEnqueue(ubQueue* queue, const char* item)
{
    if (ubqIsFull(queue)) {
        queue->capacity *= 2;
        queue->array = realloc(queue->array, queue->capacity * sizeof(int));
    }
    queue->rear = (queue->rear + 1)% queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
//    printf("%s enqueued to queue of coEditor\n", item);
}

// Function to remove an item from queue.
// It changes front and size
char* ubqDequeue(ubQueue* queue)
{
    char* item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
//    printf("%s removed from queue of coEditor\n", item);
    return item;
}

void ubqDelqueue(ubQueue* queue){
    free(queue->array);

}