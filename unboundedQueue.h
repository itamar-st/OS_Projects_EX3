//
// Created by itamar on 02/06/2022.
//

#ifndef EX3_UNBOUNDEDQUEUE_H
#define EX3_UNBOUNDEDQUEUE_H
// A structure to represent a queue

struct ubQueue {
    int front, rear, size;
    unsigned capacity;
    int* array;
};
struct ubQueue* ubqCreateQueue(unsigned capacity);
int ubqIsFull(struct ubQueue* queue);
int ubqisEmpty(struct ubQueue* queue);
void ubqEnqueue(struct ubQueue* queue, int item);
int ubqDequeue(struct ubQueue* queue);
int ubqFront(struct ubQueue* queue);
int ubqRear(struct ubQueue* queue);

#endif //EX3_UNBOUNDEDQUEUE_H
