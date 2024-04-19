#include "Includes/defs.h"

PCB *initializeProcess(Process p)
{
    PCB *pcb = (PCB *)malloc(sizeof(PCB));
    pcb->id = p.id;
    pcb->arrivalTime = p.arrivalTime;
    pcb->remainingTime = p.runningTime;
    pcb->priority = p.priority;
    pcb->runningTime = p.runningTime;
    pcb->state = STARTED;
    pcb->waitingTime = 0;
    pcb->preemptedAt = 0;
    return pcb;
}
