#include <stdio.h>
#include <stdlib.h>
#include "../Includes/defs.h"

#define NFIELDS 9

const char* headers[NFIELDS] = {
    "#At time\t%d\t",
    "process\t%d\t",
    "%s\t",
    "arr\t%d\t",
    "total\t%d\t",
    "remain\t%d\t",
    "wait\t%d\t",
    "TA\t%d\t",
    "WTA\t%d"
};

void intializeOutputFile(FILE **fptr, char *fileName)
{
    *fptr = fopen(fileName, "a+");
}

void intializeLogFile(FILE **fptr)
{
    intializeOutputFile(fptr,"scheduler.log");
    int headersLength = 14;
    
    char* headers[] = {
        "#At",
        "time",
        "x ",
        "process",
        "y ",
        "state    ",
        "arr",
        "w ",
        "total",
        "z ",
        "remain",
        "y ",
        "wait",
        "k "
    };

    for(int i = 0; i < 14; i++){
        fprintf(*fptr,headers[i]);

        if(i != headersLength - 1)
            fprintf(*fptr,"\t");
    }

    fprintf(*fptr,"\n");
}

void intializePerfFile(FILE **fptr)
{
    intializeOutputFile(fptr, "scheduler.perf");
}

void printLog(FILE* fptr, PCB *running, int clock)
{
    int nfields = running->state == FINISHED ? NFIELDS : NFIELDS - 2;

    int array[] = {clock,
                   running->id,
                   running->state,
                   running->arrivalTime,
                   running->runningTime - running->remainingTime,
                   running->remainingTime,
                   running->waitingTime,
                   running->turnaround,
                   running->weightedTurnaround
                };

    for(int i = 0; i < nfields; i++)
    {
        if(i == 2)
            fprintf(fptr,headers[i],state(running->state));
        else
            fprintf(fptr,headers[i],array[i]);
    }

    fprintf(fptr,"\n");
}

void printPerf(FILE *fptr, float utilization, float avgWTA, float avgWaiting, float stdWTA)
{
    fprintf(fptr, "CPU utilization = %f\n", utilization);
    fprintf(fptr, "Avg WTA = %f\n", avgWTA);
    fprintf(fptr, "Avg Waiting = %f\n", avgWaiting);
    fprintf(fptr, "Std WTA= %f\n", stdWTA);
}
