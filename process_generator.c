#include "userInput.h"
//#include "inputFile.h"
#include <unistd.h>
#include <string.h>
#include "ProcessQueue.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef _SchedulingAlgorithm
#define _SchedulingAlgorithm
#define HPF 1
#define STRN 2
#define RR 3
#endif

ProcessQueue* pQueue = (ProcessQueue*)malloc(sizeof(ProcessQueue));

void clearResources(int);
/*

        To run this function you need to pass an argument which is the absolute path of the project
        like : /home/rabie/Desktop/Tux-Edo

*/
int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    
    initializeProcessQueue(pQueue);

    // 1. Reading input file
    inputFile();

    // 2. Taking user input for choice of scheduling algorithm and parameters if needed
    int selectedAlgo = userInput();

    // 3. Initiating and creating scheduler and clock processes.
    char *absolutePath = argv[1];
    system("gcc clk.c -o clk");
    system("gcc scheduler.c -o scheduler");

    // Forking clock process and changing core image
    int pid = fork();
    if (pid == -1)
    {
        perror("Error in forking");
        exit(-1);
    }
    else if (pid == 0)
    {
        execl(strcat(argv[1], "/clk"), "clk", NULL);
    }

    // Forking scheduler process and changing core image
    pid = fork();
    if (pid == -1)
    {
        perror("Error in forking");
        exit(-1);
    }
    else if (pid == 0)
    {
        execl(strcat(argv[1], "/scheduler"), "scheduler", NULL);
    }

    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    //int x = getClk();
    
    //printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
}

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

            enqueue(pQueue, newProcess);
        }
    }
}