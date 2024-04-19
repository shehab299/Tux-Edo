#include "headers.h"
#include "process.h"

typedef struct ProcessQueue {

    Process* head;
    Process* tail;

    int count;
};

void initializeProcessQueue(ProcessQueue* pq) {
    pq->head= NULL;
    pq->tail = NULL;
}