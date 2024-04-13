#ifndef _PCB
#define _PCB
typedef struct PCB
{
  struct PCB *next;
  int pid;         // id from forking
  int id;          // id from input file
  int arrivalTime; // the time the process arrived at
  int runningTime; // total time needed by the process to finish executing
  int priority;
  int waitingTime;   // total time the process spend at readu and block queue
  int executionTime; //
  int remainingTime;
  int startingTime;
  int preemptedAt; // last Time the process preempted at
  int finishTime;
  int turnaround;
  int weightedTurnaround;
  int state; // 10->started    11->resumed    12->stopped    13->finished   14->running
} PCB;
#endif
