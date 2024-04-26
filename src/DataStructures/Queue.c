#include "Queue.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct Node{
    void* element;
    struct Node* next;
} Node;

typedef struct Queue {
    struct Node* head;
    struct Node* tail;
    int count;
} Queue;

void* peek(Queue* q) {
    if (q->count == 0)  return (void *) NULL;

    return q->head->element;
}


void dequeue(Queue* q) {
    if (q->head == NULL) return;

    if (q->head == q->tail)
        q->tail = q->head->next;

    Node* oldNode = q->head;
    q->head = q->head->next;
    q->count--;

    free(oldNode);
}

void enqueue(Queue* q, void* element) {

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
        printf("Enqueued first element!");
        return;
    }

    q->tail->next = newNode;
    q->tail = newNode;
    q->count++;
    printf("Enqueued element!");
}

Queue* createQueue() {

    Queue* q = (Queue*) malloc(sizeof(Queue));

    q->head= NULL;
    q->tail = NULL;
    q->count = 0;

    return q;
}

int getSize(Queue* q){
    return q->count;
}