#include "PCB.h"
typedef struct CircularQueue
{
  struct PCB *head;
  struct PCB *tail;
  int count;
} CircularQueue;
CircularQueue createCircularQueue();
void addProcess(CircularQueue *queue, PCB *pcb);
void switchRunningProcess(CircularQueue *queue);
PCB *getRunningProcess(CircularQueue *queue);
int terminateProcess(CircularQueue *queue);
void printLog(PCB *running, int clock);