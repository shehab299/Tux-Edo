#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NFIELDS 9

struct IO{
    FILE* logFile;
    FILE* perfFile;
};

const char* headers[NFIELDS] = {
    "At time\t%d\t",
    "process\t%d\t",
    "%s\t",
    "arr\t%d\t",
    "total\t%d\t",
    "remain\t%d\t",
    "wait\t%d\t",
    "TA\t%d\t",
    "WTA\t%d"
};

void intializeLogFile(struct IO* output)
{
    output->logFile = safe_fopen("scheduler.log","w+");
    
    char* headers[14] = {
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
        fputs(headers[i],output->logFile);

        if(i != 13)
            fprintf(output->logFile,"\t");
    }

    fprintf(output->logFile,"\n");
}

void intializePerfFile(struct IO* output)
{
    output->perfFile = safe_fopen("scheduler.perf","w+");
}


struct IO* createIO(){
    struct IO* output = malloc(sizeof(struct IO));
    intializeLogFile(output);
    intializePerfFile(output);
    return output;
}


void printLog(struct IO* output, PCB *running, int clock)
{
    int nfields = running->state == FINISHED ? NFIELDS : NFIELDS - 2;

    int array[] = {clock,
                   running->id,
                   running->state,
                   running->arrivalTime,
                   running->runningTime,
                   running->remainingTime,
                   running->waitingTime,
                   running->turnaround,
                };

    for(int i = 0; i < nfields; i++)
    {
        if(i == 2)
            fprintf(output->logFile,headers[i],state(running->state));
        else if (i==8)
        {
            if (fmod(running->weightedTurnaround,1.0) == 0.0)
                fprintf(output->logFile, "WTA\t%.0f", running->weightedTurnaround);
            else if (fmod(running->weightedTurnaround*10, 1.0) == 0.0)
                fprintf(output->logFile, "WTA\t%.1f", running->weightedTurnaround);
                else
                fprintf(output->logFile, "WTA\t%.2f", running->weightedTurnaround);
        }
        else
            fprintf(output->logFile,headers[i],array[i]);
    }

    fputs("\n",output->logFile);
}

void printPerf(struct IO* output, float utilization, float avgWTA, float avgWaiting, float stdWTA)
{
    fprintf(output->perfFile, "CPU utilization = %.2f\%\n", utilization);
    fprintf(output->perfFile, "Avg WTA = %.2f\n", avgWTA);
    fprintf(output->perfFile, "Avg Waiting = %.2f\n", avgWaiting);
    fprintf(output->perfFile, "Std WTA = %.2f\n", stdWTA);
}
