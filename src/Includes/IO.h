#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NFIELDS 6

struct IO
{
    FILE *logFile;
    FILE *perfFile;
};

const char *headers[NFIELDS] = {
    "At time\t%d\t",
    "%s\t",
    "%d\t",
    "bytes\tfor\tprocess\t%d\t",
    "from\t%d\t",
    "to\t%d\t",
};

void intializeLogFile(struct IO *output)
{
    output->logFile = safe_fopen("scheduler.log", "w+");

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
        "j"};

    for (int i = 0; i < 13; i++)
    {
        fputs(headers[i], output->logFile);

        if (i != 12)
            fprintf(output->logFile, "\t");
    }

    fprintf(output->logFile, "\n");
}

void intializePerfFile(struct IO *output)
{
    output->perfFile = safe_fopen("scheduler.perf", "w+");
}

struct IO *createIO()
{
    struct IO *output = malloc(sizeof(struct IO));
    intializeLogFile(output);
    intializePerfFile(output);
    return output;
}

void printLog(struct IO *output, PCB *running, int clock)
{

    int array[] = {clock,
                   running->allocationState,
                   running->memsize,
                   running->id,
                   running->startLocation,
                   running->endLocation};

    for (int i = 0; i < NFIELDS; i++)
    {
        if (i == 1)
            fprintf(output->logFile, headers[i], allocationState(running->allocationState));
        else
            fprintf(output->logFile, headers[i], array[i]);
    }

    fputs("\n", output->logFile);
}

void printPerf(struct IO *output, float utilization, float avgWTA, float avgWaiting, float stdWTA)
{
    fprintf(output->perfFile, "CPU utilization = %.2f\%\n", utilization);
    fprintf(output->perfFile, "Avg WTA = %.2f\n", avgWTA);
    fprintf(output->perfFile, "Avg Waiting = %.2f\n", avgWaiting);
    fprintf(output->perfFile, "Std WTA = %.2f\n", stdWTA);
}
