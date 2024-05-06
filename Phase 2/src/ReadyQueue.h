#include <stdlib.h>
#include "DataStructures/Queue.h"
#include "DataStructures/PriorityQueue.h"
#include "Includes/defs.h"

struct ReadyQueue
{
    void *datastructre;
    void (*enqueue)(void *, void *);
    void (*dequeue)(void *);
    void *(*top)(void *);
    int (*size)(void *);
    bool (*empty)(void *);
};

typedef struct ReadyQueue ReadyQueue;

bool comparePriority(void *a, void *b)
{
    PCB *processA = (PCB *)a;
    PCB *processB = (PCB *)b;
    return processA->priority < processB->priority;
}

bool compareRemaining(void *a, void *b)
{
    PCB *processA = (PCB *)a;
    PCB *processB = (PCB *)b;
    return processA->remainingTime <= processB->remainingTime;
}

ReadyQueue *createReadyQueue(enum SchedulingAlgorithm type)
{

    ReadyQueue *ready = malloc(sizeof(ReadyQueue));

    if (type == RR)
    {
        ready->datastructre = createQueue();
        ready->enqueue = q_enqueue;
        ready->dequeue = q_dequeue;
        ready->empty = q_empty;
        ready->size = q_getSize;
        ready->top = q_peek;
    }
    else if (type == HPF || type == SRTN)
    {

        if (type == HPF)
            ready->datastructre = pq_create(10, comparePriority);
        else if (type == SRTN)
            ready->datastructre = pq_create(10, compareRemaining);

        ready->enqueue = pq_enqueue;
        ready->dequeue = pq_dequeue;
        ready->size = pq_size;
        ready->top = pq_top;
        ready->empty = pq_empty;
    }

    return ready;
}

void enqueue(PCB *process, ReadyQueue *ready)
{
    return ready->enqueue(ready->datastructre,process);
}

void dequeue(ReadyQueue *ready)
{
    return ready->dequeue(ready->datastructre);
}

PCB *peek(ReadyQueue *ready)
{
    return ready->top(ready->datastructre);
}

int size(ReadyQueue *ready)
{
    return ready->size(ready->datastructre);
}

bool empty(ReadyQueue *ready)
{
    return ready->empty(ready->datastructre);
}
