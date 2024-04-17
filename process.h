#include "PCB.h"
#ifndef _Process
#define _Process
typedef struct Process
{
    int id;
    int arrivalTime;
    int runningTime;
    int priority;
} Process;
#endif

#ifndef __ProcessStates
#define __ProcessStates
#define started 10
#define resumed 11
#define stopped 12
#define finished 13
#define runningState 14
#endif

PCB *initializeProcess(Process p)
{
    PCB *pcb = (PCB *)malloc(sizeof(PCB));
    pcb->id = p.id;
    pcb->arrivalTime = p.arrivalTime;
    pcb->remainingTime = p.runningTime;
    pcb->priority = p.priority;
    pcb->runningTime = p.runningTime;
    pcb->state = started;
    pcb->waitingTime = 0;
    pcb->preemptedAt = 0;
    return pcb;
}