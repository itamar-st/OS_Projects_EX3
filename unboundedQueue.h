//
// Created by itamar on 02/06/2022.
//

#ifndef EX3_UNBOUNDEDQUEUE_H
#define EX3_UNBOUNDEDQUEUE_H
// A structure to represent a queue

typedef struct ubQueue {
    int front, rear, size;
    unsigned capacity;
    int* array;
} ubQueue;
struct ubQueue* ubqCreateQueue(unsigned capacity);
int ubqIsFull( ubQueue* queue);
int ubqisEmpty( ubQueue* queue);
void ubqEnqueue( ubQueue* queue, int item);
int ubqDequeue( ubQueue* queue);
int ubqFront( ubQueue* queue);
int ubqRear( ubQueue* queue);

#endif //EX3_UNBOUNDEDQUEUE_H
