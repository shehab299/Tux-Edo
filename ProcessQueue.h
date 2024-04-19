#ifndef _PROCESSQ_
#define _PROCESSQ_
#include "headers.h"
#include "process.h"

typedef struct ProcessQueue {

    ProcessNode* head;
    ProcessNode* tail;

    int count;
} ProcessQueue;

typedef struct ProcessNode {
    Process* proc;
    ProcessNode* next;
} ProcessNode;

#endif

void initializeProcessQueue(ProcessQueue* pq) {
    pq->head= NULL;
    pq->tail = NULL;
}

void enqueue(ProcessQueue* pq, Process* p) {
    if (pq == NULL || p == NULL)    return;

    // Creating new process node
    ProcessNode* newNode = (ProcessNode*) malloc (sizeof(ProcessNode));
    if (newNode == NULL)    return;     // if malloc fails to allocate memory
    newNode->proc = p;
    newNode->next = NULL;

    // If the queue is initially empty
    if (pq->head == NULL || pq->tail == NULL) {
        // Since there is only one process, head and tail will point to the same node
        pq->head = newNode;        
        pq->tail = pq->head;
        return;
    }

    pq->tail->next = newNode;
    pq->tail = newNode;
}

void dequeue(ProcessQueue* pq) {
    // If there are no nodes to dequeue, return
    if (pq->head == NULL) return;

    if (pq->head == pq->tail)
        pq->tail = pq->head->next;

    ProcessNode* oldNode = pq->head;
    pq->head = pq->head->next;

    // The function currently deleted the old node upon dequeueing. This may be reconsidered.
    delete oldNode;
}