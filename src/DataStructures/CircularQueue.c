#include "CircularQueue.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

struct Node
{
  void* data;
  struct Node* next;
};

typedef struct Node Node;

Node* createNode(void* element){

  Node* newNode = malloc(sizeof(Node));
  newNode->data = element;

  return newNode;
}

struct CircularQueue
{
  Node* head;
  Node* tail; 

  int count;
};


void* cq_create()
{
  CircularQueue* queue = malloc(sizeof(CircularQueue));
  queue->head = NULL;
  queue->tail = NULL;
  queue->count = 0;

  return (void*) queue;
}

void cq_enqueue(void* _queue, void* element)
{

  CircularQueue* queue = (CircularQueue*) _queue;

  if(queue == NULL)
  {
    printf("NULL POINTER DETECTED");
    exit(-1); 
  }

  Node* newNode = createNode(element);
  
  if(queue->head == queue->tail && queue->head == NULL)
  {
    queue->head = queue->tail = newNode;
    queue->count = 1;
    return;
  }
  
  queue->tail->next = newNode;
  queue->tail = newNode;
  queue->count++;

  return;
}

void cq_dequeue(void* _queue){

  CircularQueue* queue = (CircularQueue*) _queue;

  if(queue == NULL)
  {
    printf("NULL POINTER DETECTED");
    exit(-1); 
  }

  if(queue->head == queue->tail){
    queue->head = queue->tail = NULL;
    queue->count = 0;
    return;
  }

  Node* temp = queue->head;
  queue->head = queue->head->next;
  queue->count--;
  
  free(temp);
}

bool cq_empty(void* _queue){

  CircularQueue* queue = (CircularQueue*) _queue;

  if(queue == NULL)
  {
    printf("NULL POINTER DETECTED");
    exit(-1); 
  }

  return queue->count == 0;
}

int cq_size(void* _queue){

  CircularQueue* queue = (CircularQueue*) _queue;

  if(queue == NULL)
  {
    printf("NULL POINTER DETECTED");
    exit(-1); 
  }

  return queue->count;
}

void* cq_top(void* _queue){
  
  CircularQueue* queue = (CircularQueue*) _queue;

  if(queue == NULL)
  {
    printf("NULL POINTER DETECTED");
    exit(-1); 
  }

  if(queue->count == 0)
  {
    printf("Circular Queue Is Empty");
    exit(-1);
  }

  return (*(queue->head)).data;
}

