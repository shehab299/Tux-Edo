#include "../src/DataStructures/PriorityQueue.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct dummy_{
    int time;
    int order;
};

typedef struct dummy_ dummy;

bool comp(void* x , void* y)
{
    dummy* d1 = (dummy*) x;
    dummy* d2 = (dummy*) y;
    return d1->time > d2->time;
}


int main(int argc, char const *argv[])
{

    PriorityQueue* pq = (PriorityQueue*) pq_create(150 , comp);
    dummy* array[3];

    array[0] = malloc(sizeof(dummy));
    array[1] = malloc(sizeof(dummy));
    array[2] = malloc(sizeof(dummy));

    array[0]->time = 0;
    array[1]->time = 1;
    array[2]->time = 2;
    
    pq_enqueue(array[0],pq);
    pq_enqueue(array[1],pq);
    pq_enqueue(array[2],pq);

    pq_dequeue(pq);
    pq_dequeue(pq);

    dummy* x = pq_top(pq);
    printf("%d \n" , x->time);

    return 0;
}
