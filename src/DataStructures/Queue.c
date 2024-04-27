#include "Queue.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct Node{
    void* element;
    struct Node* next;
} Node;

typedef struct Queue {
    struct Node* head;
    struct Node* tail;
    int count;
} Queue;

void* q_peek(void* q_) {
    
    Queue* q = (Queue*) q_;

    if (q->count == 0)  return (void *) NULL;

    return q->head->element;
}


void q_dequeue(void* q_) {
    Queue* q = (Queue*) q_;
    
    if (q->head == NULL) return;

    if (q->head == q->tail)
        q->tail = q->head->next;

    Node* oldNode = q->head;
    q->head = q->head->next;
    q->count--;

    free(oldNode);
}

void q_enqueue(void* q_, void* element) {

    Queue* q = (Queue*) q_;

    if (q == NULL || element == NULL)    
        return;

    Node* newNode = (Node*) malloc(sizeof(Node));

    if (newNode == NULL)    
        return;

    newNode->element = element;
    newNode->next = NULL;

    if (q->head == NULL || q->tail == NULL) {
        q->head = newNode;        
        q->tail = q->head;
        q->count++;
        // printf("Enqueued first element!");
        return;
    }

    q->tail->next = newNode;
    q->tail = newNode;
    q->count++;
    printf("Enqueued element!");
}

void* createQueue() {

    Queue* q = (Queue*) malloc(sizeof(Queue));

    q->head= NULL;
    q->tail = NULL;
    q->count = 0;

    return (void*) q;
}

bool q_empty(void* q_)
{
    Queue* q = (Queue*) q_;
    return q->count == 0;
}

int q_getSize(void* q_){
    Queue* q = (Queue*) q_;
    return q->count;
}