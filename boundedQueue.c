// C program for array implementation of queue
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include "boundedQueue.h"



int ret;
// function to create a queue
// of given capacity.
// It initializes size of queue as 0
bQueue* createQueue(unsigned capacity)
{
    bQueue* queue = ( bQueue*)malloc(sizeof(bQueue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;

    // This is important, see the enqueue
    queue->rear = capacity - 1;
    queue->array = (char**)malloc(queue->capacity * sizeof(char*));

    return queue;
}

// Function to add an item to the queue.
// It changes rear and size
void enqueue(bQueue* queue, const char* item)
{
//    char itemForQueue[32];
//    strcpy(itemForQueue, item);
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
//    printf("%s enqueued to queue\n" ,item);
}

// Function to remove an item from queue.
// It changes front and size
char* dequeue(bQueue* queue)
{
    const char* item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
//    printf("%s removed from queue\n", item);

    return item;
}

void delqueue(bQueue* queue){
    free(queue->array);

}
