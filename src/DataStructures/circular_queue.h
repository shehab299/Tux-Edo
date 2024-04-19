#include "../Includes/defs.h"

#ifndef _CircularQueue
#define _CircularQueue
typedef struct CircularQueue
{
  struct PCB *head;
  struct PCB *actualTail;  // the last pcb added to the queue
  struct PCB *virtualTail; // While i am traversing the queue the pcb before head is virtual tail

  int count;
} CircularQueue;
#endif
CircularQueue createCircularQueue();
void addProcess(CircularQueue *queue, PCB *pcb);
void switchRunningProcess(CircularQueue *queue);
PCB *getRunningProcess(CircularQueue *queue);
int terminateProcess(CircularQueue *queue);