#include "queue.h"

QUEUE* new_queue(size_t sz) {
    QUEUE* q = (QUEUE* )malloc(sizeof(QUEUE));
    q->size = sz;
    q->array = malloc(sz);
    q->front = 0;
    q->end = 0;
    return q;
}

void free_queue(QUEUE* q) 
{
    free(q->array);
    free(q);
}

void enqueue(QUEUE* q, int v) 
{
    q->array[q->end] = v;
    q->end = (q->end + 1) % q->size;
}

int dequeue(QUEUE* q) 
{
    int v = q->array[q->front];
    q->front = (q->front + 1) % q->size;
    return v;
}
