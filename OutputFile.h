#include <stdio.h>
#include <stdlib.h>
#include "PCB.h"

void intializeOutputFile(FILE **fptr, char *fileName)
{
    *fptr = fopen(fileName, "w");
    fclose(*fptr);
    *fptr = fopen(fileName, "a");
}

void intializeLogFile(FILE **fptr)
{
    intializeOutputFile(fptr, "scheduler.log");
    fprintf(*fptr, "#At\ttime\tx\tprocess\ty\tstate\tarr\tw\ttotal\tz\tremain\ty\twait\tk\n");
}

void intializePerfFile(FILE **fptr)
{
    intializeOutputFile(fptr, "scheduler.perf");
}

void printLog(FILE *fptr, PCB *running, int clock)
{

    if (running->state == 13)
        fprintf(fptr, "At\ttime\t%d\tprocess\t%d\tfinished\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\tTA\t%d\tWTA\t%.2f\n", clock, running->id, running->arrivalTime, running->runningTime, running->remainingTime, running->waitingTime, running->turnaround, running->weightedTurnaround);
    else
        fprintf(fptr, "At\ttime\t%d\tprocess\t%d\t%s\tarr\t%d\ttotal\t%d\tremain\t%d\twait\t%d\n", clock, running->id, (running->state == 10) ? "started" : (running->state == 11) ? "resumed"
                                                                                                                                                                                   : "stopped",
                running->arrivalTime, running->runningTime, running->remainingTime, running->waitingTime);
}

void printPerf(FILE *fptr, float utilization, float avgWTA, float avgWaiting, float stdWTA)
{
    fprintf(fptr, "CPU utilization = %f\n", utilization);
    fprintf(fptr, "Avg WTA = %f\n", avgWTA);
    fprintf(fptr, "Avg Waiting = %f\n", avgWaiting);
    fprintf(fptr, "Std WTA= %f\n", stdWTA);
}