#include "../Includes/defs.h"

typedef struct Node{
    void* element;
    struct Node* next;

} Node;

typedef struct {

    Node* head;
    Node* tail;
    int count;

} Queue;


Queue* createQueue() {

    Queue* q = (Queue*) malloc(sizeof(Queue));

    q->head= NULL;
    q->tail = NULL;
    q->count = 0;
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
        return;
    }

    q->tail->next = newNode;
    q->tail = newNode;
    q->count++;
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
