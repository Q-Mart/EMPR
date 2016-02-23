#include "empr.h"

typedef struct 
{
    void**      array;
    uint32_t    size;
    uint32_t    max;
    uint32_t    front;
    uint32_t    end;
} QUEUE;

QUEUE*  new_queue(uint32_t);
void    free_queue(QUEUE*);
void    enqueue(QUEUE*, void*);
void*   ptr_dequeue(QUEUE*);

#define dequeue(q, type) *(type* )ptr_dequeue(q)
