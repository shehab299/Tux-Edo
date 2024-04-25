#include <stdlib.h>
#include <DataStructures/CircularQueue.h>
#include <DataStructures/PriorityQueue.h>
#include "Includes/defs.h"

struct ReadyQueue{
    void* datastructre;
    void (*enqueue)(void*,void*);
    void (*dequeue)(void*);
    void* (*top)(void*);
    int (*size)(void*);
    bool (*empty)(void*);
};

typedef struct ReadyQueue ReadyQueue;

enum Type{
    Circular = 1,
    Priority
};

typedef enum Type Type;

bool comparePriority(void *a, void *b)
{
    PCB *processA = (PCB *)a;
    PCB *processB = (PCB *)b;
    return processA->priority < processB->priority;
}

bool compareRemaining(void* a, void* b)
{
    PCB *processA = (PCB *)a;
    PCB *processB = (PCB *)b;
    return processA->remainingTime < processB->remainingTime;
}


ReadyQueue* createReadyQueue(Type type){

    ReadyQueue* ready = malloc(sizeof(ReadyQueue));

    if(type == 1){
        ready->datastructre = cq_create();
        ready->enqueue = cq_enqueue;
        ready->dequeue = cq_dequeue;
        ready->empty = cq_empty;
        ready->size = cq_size;
        ready->top = cq_top;
    }
    else if(type == 2 || type == 3){

        if(type == 2)
            ready->datastructre = pq_create(10,comparePriority);
        else if(type == 3)
            ready->datastructre = pq_create(10,compareRemaining);

        ready->enqueue = pq_enqueue;
        ready->dequeue = pq_dequeue;
        ready->size = pq_size;
        ready->top = pq_top;
        ready->empty = pq_empty;
    }

    return ready;
}

void enqueue(ReadyQueue* ready,PCB* process){
    return ready->enqueue(ready->datastructre,process);
}

void dequeue(ReadyQueue* ready){
    return ready->dequeue(ready->datastructre);
}

PCB* top(ReadyQueue* ready){
    return ready->top(ready->datastructre);
}

int size(ReadyQueue* ready){
    return ready->size(ready->datastructre);
}

bool empty(ReadyQueue* ready){
    return ready->empty(ready->datastructre);
}

