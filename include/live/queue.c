#include "queue.h"

QUEUE* new_queue(size_t sz) {
    QUEUE* q = (QUEUE* )malloc(sizeof(QUEUE));
    q->size = sz;
    q->array = malloc(sz);
    q->front = 0;
    q->end = 0;
    return q;
}

/* Warning: This will only free the array
 * it will not free pointers within the array
 * since it would be impossible to know when to do that
 * without reference tracking
 */
void free_queue(QUEUE* q) 
{
    free(q->array);
    free(q);
}

void enqueue(QUEUE* q, void* v_ptr) 
{
    q->array[q->end] = v_ptr;
    q->end = (q->end + 1) % q->size;
}

void* dequeue(QUEUE* q) 
{
    void* v_ptr = q->array[q->front];
    q->front = (q->front + 1) % q->size;
    return v_ptr;
}
