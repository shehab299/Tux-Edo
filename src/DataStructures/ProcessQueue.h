#ifndef _PROCESSQ_
#define _PROCESSQ_
#include "../Includes/defs.h"

typedef struct ProcessNode {

    Process* proc;
    struct ProcessNode* next;

} ProcessNode;
typedef struct ProcessQueue {

    ProcessNode* head;
    ProcessNode* tail;
    int count;

} ProcessQueue;

#endif

void initializeProcessQueue(ProcessQueue* pq) {
    pq->head= NULL;
    pq->tail = NULL;
    pq->count = 0;
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
        pq->count++;
        printf("Successfully enqueued process %d.\n", p->id);
        return;
    }

    pq->tail->next = newNode;
    pq->tail = newNode;
    pq->count++;
    printf("Successfully enqueued process %d.\n", p->id);
}

void dequeue(ProcessQueue* pq) {
    // If there are no nodes to dequeue, return
    if (pq->head == NULL) return;

    if (pq->head == pq->tail)
        pq->tail = pq->head->next;

    ProcessNode* oldNode = pq->head;
    pq->head = pq->head->next;
    pq->count--;

    // The function currently deleted the old node upon dequeueing. This may be reconsidered.
    free(oldNode);

    printf("Successfully dequeued process.\n");
}