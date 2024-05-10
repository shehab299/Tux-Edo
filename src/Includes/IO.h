#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NSCHEDFIELDS 9
#define NMEMFIELDS 6

struct IO
{
    FILE *memFile;
    FILE *schedulerFile;
    FILE *perfFile;
};

const char *memHeaders[NMEMFIELDS] = {
    " At time\t%d\t",
    "%s\t",
    "%d\t",
    "bytes\tfor\tprocess\t%d\t",
    "from\t%d\t",
    "to\t%d\t",
};

const char *schedulerHeaders[NSCHEDFIELDS] = {
    " At time\t%d\t",
    "process\t%d\t",
    "%s\t",
    "arr\t%d\t",
    "total\t%d\t",
    "remain\t%d\t",
    "wait\t%d\t",
    "TA\t%d\t",
    "WTA\t%d"
};


void intializeSchedulerFile(struct IO* output)
{
    output->schedulerFile = safe_fopen("scheduler.log","w+");
    
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
        fputs(headers[i],output->schedulerFile);

        if(i != 13)
            fprintf(output->schedulerFile,"\t");
    }

    fprintf(output->schedulerFile,"\n");
    printf("Scheduler file initialized\n");
}

void intializeMemFile(struct IO *output)
{

    output->memFile = safe_fopen("memory.log", "w+");

    char *headers[13] = {
        "#At",
        "time",
        "x ",
        "allocated",
        "y",
        "bytes",
        "for",
        "process",
        "z",
        "from",
        "i",
        "to",
        "j"
    };

    for (int i = 0; i < 13; i++)
    {
        fputs(headers[i], output->memFile);

        if (i != 12)
            fprintf(output->memFile, "\t");
    }

    fprintf(output->memFile, "\n");
    printf("Memory file initialized\n");
}

void intializePerfFile(struct IO *output)
{
    output->perfFile = safe_fopen("scheduler.perf", "w+");
    printf("Performance file initialized\n");
}

struct IO *createIO()
{
    struct IO *output = malloc(sizeof(struct IO));

    intializeMemFile(output);
    intializeSchedulerFile(output);
    intializePerfFile(output);

    return output;
}

void schedulerLog(struct IO* output, PCB *running, int clock)
{
    int nfields = running->state == FINISHED ? NSCHEDFIELDS : NSCHEDFIELDS - 2;

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
            fprintf(output->schedulerFile,schedulerHeaders[i],state(running->state));
        else if (i==8)
        {
            if (fmod(running->weightedTurnaround,1.0) == 0.0)
                fprintf(output->schedulerFile, "WTA\t%.0f", running->weightedTurnaround);
            else if (fmod(running->weightedTurnaround*10, 1.0) == 0.0)
                fprintf(output->schedulerFile, "WTA\t%.1f", running->weightedTurnaround);
                else
                fprintf(output->schedulerFile, "WTA\t%.2f", running->weightedTurnaround);
        }
        else
            fprintf(output->schedulerFile,schedulerHeaders[i],array[i]);
    }

    fputs("\n",output->schedulerFile);
}

void memoryLog(struct IO *output, PCB *running, int clock)
{

    int array[] = {clock,
                   running->allocationState,
                   running->memsize,
                   running->id,
                   running->startLocation,
                   running->endLocation};

    for (int i = 0; i < NMEMFIELDS; i++)
    {
        if (i == 1)
            fprintf(output->memFile, memHeaders[i], allocationState(running->allocationState));
        else
            fprintf(output->memFile, memHeaders[i], array[i]);
    }

    fputs("\n", output->memFile);
}

void printPerf(struct IO *output, float utilization, float avgWTA, float avgWaiting, float stdWTA)
{
    fprintf(output->perfFile, "CPU utilization = %.2f%s\n", utilization , "%");
    fprintf(output->perfFile, "Avg WTA = %.2f\n", avgWTA);
    fprintf(output->perfFile, "Avg Waiting = %.2f\n", avgWaiting);
    fprintf(output->perfFile, "Std WTA = %.2f\n", stdWTA);
}
