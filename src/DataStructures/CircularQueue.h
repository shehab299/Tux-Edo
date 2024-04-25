#pragma once

#include <stdbool.h>
#include <stdlib.h>


typedef struct CircularQueue CircularQueue;

void* cq_create();

void cq_enqueue(void* queue, void* element);

void cq_dequeue(void* queue);

int cq_size(void* queue);

void* cq_top(void* queue);

bool cq_empty(void* queue);

