#include <stdio.h>
#include <stdlib.h>

void inputFile()
{
    Process* newProcess;
    FILE *fptr;
    fptr = fopen("processes.txt", "r");
    if (fptr == NULL)
        exit(-1);
    int id, arrival, runtime, priority;
    char ch;

    while ((ch = fgetc(fptr)) != EOF)
    {
        if (ch == '#')
        {
            fscanf(fptr, "%*[^\n]");
        }
        else
        {
            ungetc(ch, fptr);
            fscanf(fptr, "%d %d %d %d", &id, &arrival, &runtime, &priority);

            newProcess = (Process *)malloc(sizeof(Process));
            newProcess->arrivalTime = arrival;
            newProcess->id = id;
            newProcess->runningTime = runtime;
            newProcess->priority = priority;
        }
    }
}