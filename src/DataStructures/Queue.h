#pragma once

#include <stdbool.h>

typedef struct Queue Queue;
typedef struct Node Node;

void q_dequeue(void* q);
void q_enqueue(void* q, void* element);
void* q_peek(void*);
void* createQueue();
int q_getSize(void*);
bool q_empty(void*);


