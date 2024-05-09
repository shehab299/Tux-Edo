#include "PriorityQueue.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

struct PriorityQueue
{
    void **array;
    int count;
    int capacity;
    bool (*comp)(void *, void *);
};

void resize(PriorityQueue **queue)
{
    (*queue)->capacity *= 2;
    PriorityQueue *newQueue = pq_create((*queue)->capacity, (*queue)->comp);
    for (int i = 0; i < (*queue)->count; i++)
        newQueue->array[i] = (*queue)->array[i];
    newQueue->count = (*queue)->count;
    free(*queue);
    *queue = newQueue;
}

    void *pq_create(int size, bool (*comp)(void *, void *))
    {

        PriorityQueue *q = malloc(sizeof(struct PriorityQueue));
        q->comp = comp;

        if (size <= 0)
        {
            perror("queue size can't be below zero");
            exit(-1);
        }

        q->array = malloc(sizeof(void *) * size);
        q->count = 0;
        q->capacity = size;

        return q;
}

void pq_destory(void *_queue)
{

    PriorityQueue *queue = (PriorityQueue *)_queue;

    if (queue == NULL)
    {
        perror("nothing to destory");
        exit(-1);
    }

    free(queue->array);
    free(queue);
}

int pq_size(void *_queue)
{

    PriorityQueue *queue = (PriorityQueue *)_queue;

    return (*queue).count;
}

bool pq_empty(void *_queue)
{

    PriorityQueue *queue = (PriorityQueue *)_queue;

    return (*queue).count == 0;
}

void pq_dequeue(void *_queue)
{
    pq_remove(_queue, 0);
}

void pq_remove(void *_queue, int k)
{
    PriorityQueue *queue = (PriorityQueue *)_queue;
    for (int i = k + 1; i < queue->count; i++)
    {
        queue->array[i - 1] = queue->array[i];
    }

    queue->count--;
    _queue = queue;
}

void pq_remove_element(void *_queue, void *element)
{
    PriorityQueue *queue = (PriorityQueue *)_queue;
    for (int i = 0; i < queue->count;i++)
    {
        if(queue->array[i] == element)
        {
            pq_remove(queue, i);
            break;
        }
    }
}

void pq_enqueue(void *_queue, void *k)
{

    PriorityQueue **queue = (PriorityQueue **)_queue;

    if ((*queue)->count >= (*queue)->capacity)
    {
       resize(queue);
    }
    int in = (*queue)->count;

    (*queue)->array[(*queue)->count++] = k;

    int prev = in - 1;
    while (in != 0 && (*queue)->comp(k, (*queue)->array[in - 1]))
    {
        (*queue)->array[in] = (*queue)->array[in - 1];
        in--;
    }
    (*queue)->array[in] = k;
}

void *pq_top(void *_queue)
{
    return pq_at(_queue, 0);
}

void *pq_at(void *_queue, int index)
{
    PriorityQueue *queue = (PriorityQueue *)_queue;
    if(index >= queue->count)
        return NULL;
    return queue->array[index];
}
