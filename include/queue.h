#include "empr.h"

typedef struct 
{
    int*    array;
    size_t  size;
    int     front;
    int     end;
} QUEUE;

QUEUE*  new_queue(size_t);
void    free_queue(QUEUE*);
void    enqueue(QUEUE*, int);
int     dequeue(QUEUE*);
