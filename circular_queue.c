#include "circular_queue.h"
#include <stdlib.h>
#include <stdio.h>

CircularQueue createCircularQueue()
{
  CircularQueue queue;
  queue.count = 0;
  queue.head = NULL;
  queue.tail = NULL;
  return queue;
}

void addProcess(CircularQueue *queue, PCB *pcb)
{
  if (queue->count == 0)
  {
    queue->head = pcb;
  }
  else
  {
    queue->tail->next = pcb;
  }
  queue->tail = pcb;
  queue->tail->next = queue->head;
  queue->count++;
}

void switchRunningProcess(CircularQueue *queue)
{

  queue->tail = queue->head;
  queue->head = queue->head->next;
}

PCB *getRunningProcess(CircularQueue *queue) { return queue->head; }

int terminateProcess(CircularQueue *queue)
{

  if (queue->count > 0)
  {
    PCB *removed = queue->head;
    queue->head = queue->head->next;
    queue->tail->next = queue->head;
    queue->count--;
    free(removed);
    if (queue->count == 0)
    {
      queue->head = NULL;
      queue->tail = NULL;
    }
  }
  return queue->count;
}

void printLog(PCB *running, int clock)
{
  if (running->state == 10)
    printf("At  time  %d  process   %d  started  arr   %d  total   %d  remain  %d  wait %d \n", clock, running->id, running->arrivalTime, running->runningTime, running->remainingTime, running->waitingTime);
  else if (running->state == 11)
    printf("At  time  %d  process   %d  resumed  arr   %d  total   %d  remain  %d  wait %d \n", clock, running->id, running->arrivalTime, running->runningTime, running->remainingTime, running->waitingTime);
  else if (running->state == 12)
    printf("At  time  %d  process   %d  stopped  arr   %d  total   %d  remain  %d  wait %d \n", clock, running->id, running->arrivalTime, running->runningTime, running->remainingTime, running->waitingTime);
  else
    printf("At  time  %d  process   %d  finished  arr   %d  total   %d  remain  %d  wait %d  TA   %d  WTA   %d  \n", clock, running->id, running->arrivalTime, running->runningTime, running->remainingTime, running->waitingTime, running->turnaround, running->weightedTurnaround);
}
