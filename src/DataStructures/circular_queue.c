#include "circular_queue.h"
#include <stdlib.h>
#include <stdio.h>

CircularQueue createCircularQueue()
{
  CircularQueue queue;
  queue.count = 0;
  queue.head = NULL;
  queue.virtualTail = NULL;
  queue.actualTail = NULL;
  return queue;
}

void addProcess(CircularQueue *queue, PCB *pcb)
{
  if (queue->count == 0)
  {
    queue->head = pcb;
    queue->virtualTail = pcb;
  }
  else
  {
    queue->actualTail->next = pcb;
  }
  queue->actualTail = pcb;
  queue->actualTail->next = queue->virtualTail;
  queue->count++;
}

void switchRunningProcess(CircularQueue *queue)
{
  if (queue->count)
  {
    queue->virtualTail = queue->head;
    queue->head = queue->head->next;
  }
}

PCB *getRunningProcess(CircularQueue *queue) { return queue->head; }

int terminateProcess(CircularQueue *queue)
{

  if (queue->count > 0)
  {
    PCB *removed = queue->head;
    queue->head = queue->head->next;
    queue->virtualTail->next = queue->head;
    queue->count--;
    free(removed);
    if (queue->count == 0)
    {
      queue->head = NULL;
      queue->virtualTail = NULL;
      queue->actualTail = NULL;
    }
  }
  return queue->count;
}


