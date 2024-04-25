#include "priorityQ.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

struct heap
{
    void* * minHeap;
    int count;
    int capacity;
    bool (*comp)(void*,void*);
};

int parentIndex(int in){
    return ((in -1 ) / 2);
}

int leftIndex(int in){
    return (2 * in) + 1;
}

int rightIndex(int in){
    return (2 * in) + 2;
}

void minHeapify(int key, Heap* heap)
{
    if(key >= heap->count)
        return;

    int left = leftIndex(key);
    int right = rightIndex(key);
    int smlest = key;

    int count = heap->count;


    if(left < count && heap->comp(heap->minHeap[left],heap->minHeap[smlest]))
        smlest = left;
    
    if(right < count && heap->comp(heap->minHeap[right],heap->minHeap[smlest]))
        smlest = right;

    if(smlest != key)
    {
        void* temp = heap->minHeap[key];
        heap->minHeap[key] = heap->minHeap[smlest];
        heap->minHeap[smlest] = temp;
    }
}

void* minElement(Heap* heap){
    return heap->minHeap[0];
}

void resize(Heap* heap){
    heap->minHeap = realloc(heap->minHeap,2 * heap->capacity);
    heap->capacity *= 2;
}

Heap* create_heap(int size,bool (*comp)(void*,void*)){

    Heap* x = malloc(sizeof(Heap));
    x->comp = comp;

    if(size <= 0)
    {
        perror("Heap size can't be below zero");
        exit(-1);
    }

    x->minHeap = malloc( sizeof(void*) * size );
    x->count = 0;
    x->capacity = size;

    return x;
}

void destory_heap(Heap* heap){

    if(heap == NULL)
    {
        perror("nothing to destory");
        exit(-1);
    }

    free(heap->minHeap);
    free(heap);
}

int getCount(Heap* heap){

    return (*heap).count;

}

bool isEmpty(Heap* heap){
    return (*heap).count == 0;
}

void deleteMin(Heap* heap){
    heap->minHeap[0] = heap->minHeap[heap->count - 1];
    minHeapify(0,heap);
    heap->count--;
}

void insert(void* k, Heap* heap){
    if(heap->count >= heap->capacity)
        resize(heap);

    int in = heap->count;
    heap->minHeap[heap->count++] = k;

    while(in != 0 && heap->comp(heap->minHeap[in],heap->minHeap[parentIndex(in)]))
    {
        void* temp = heap->minHeap[parentIndex(in)];
        heap->minHeap[parentIndex(in)] = heap->minHeap[in];
        heap->minHeap[in] = temp;
        in = parentIndex(in);
    }
}