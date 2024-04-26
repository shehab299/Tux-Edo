#include "PriorityQueue.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

struct PriorityQueue
{
    void** minHeap;
    int count;
    int capacity;
    bool (*comp)(void*,void*);
};

void* pq_create(int size,bool (*comp)(void*,void*)){

    PriorityQueue* q = malloc(sizeof(struct PriorityQueue));
    q->comp = comp;

    if(size <= 0)
    {
        perror("queue size can't be below zero");
        exit(-1);
    }

    q->minHeap = malloc( sizeof(void*) * size );
    q->count = 0;
    q->capacity = size;

    return q;
}

void pq_destory(void* _queue){

    PriorityQueue* queue = (PriorityQueue*) _queue;

    if(queue == NULL)
    {
        perror("nothing to destory");
        exit(-1);
    }

    free(queue->minHeap);
    free(queue);
}

int pq_size(void* _queue){
    
    PriorityQueue* queue = (PriorityQueue*) _queue;

    return (*queue).count;
}

bool pq_empty(void* _queue){

    PriorityQueue* queue = (PriorityQueue*) _queue;

    return (*queue).count == 0;
}

void pq_dequeue(void* _queue){

    PriorityQueue* queue = (PriorityQueue*) _queue;

    queue->minHeap[0] = queue->minHeap[queue->count - 1];
    minqueueify(0,queue);
    queue->count--;
}

void pq_enqueue(void* k, void* _queue){

    PriorityQueue* queue = (PriorityQueue*) _queue;
    
    if(queue->count >= queue->capacity)
        resize(queue);

    int in = queue->count;
    
    queue->minHeap[queue->count++] = k;

    while(in != 0 && queue->comp(queue->minHeap[in],queue->minHeap[parentIndex(in)]))
    {
        void* temp = queue->minHeap[parentIndex(in)];
        queue->minHeap[parentIndex(in)] = queue->minHeap[in];
        queue->minHeap[in] = temp;
        in = parentIndex(in);
    }
}


int parentIndex(int in){
    return ((in -1 ) / 2);
}

int leftIndex(int in){
    return (2 * in) + 1;
}

int rightIndex(int in){
    return (2 * in) + 2;
}

void minqueueify(int key, void* _queue)
{

    PriorityQueue* queue = (PriorityQueue*) _queue;

    if(key >= queue->count)
        return;

    int left = leftIndex(key);
    int right = rightIndex(key);
    int smlest = key;

    int count = queue->count;


    if(left < count && queue->comp(queue->minHeap[left],queue->minHeap[smlest]))
        smlest = left;
    
    if(right < count && queue->comp(queue->minHeap[right],queue->minHeap[smlest]))
        smlest = right;

    if(smlest != key)
    {
        void* temp = queue->minHeap[key];
        queue->minHeap[key] = queue->minHeap[smlest];
        queue->minHeap[smlest] = temp;
    }
}

void* pq_top(void* _queue){

    PriorityQueue* queue = (PriorityQueue*) _queue;

    return queue->minHeap[0];
}

void resize(void* _queue){

    PriorityQueue* queue = (PriorityQueue*) _queue;
    queue->minHeap = realloc(queue->minHeap,2 * queue->capacity);
    queue->capacity *= 2;
}

