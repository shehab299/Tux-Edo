#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300

#define READY 1
#define RUNNING 2
#define TERMINATED 3

typedef struct PCB
{
    struct PCB *next;
    int pid;
    int id;
    int arrivalTime;
    int runningTime;
    int priority;
    int waitingTime;
    int executionTime;
    int remainingTime;
    int startTime;
    int preemptedAt;
    int finishTime;
    int turnaround;
    float weightedTurnaround;
    int state;
} PCB;

typedef struct Process
{
    int id;
    int arrivalTime;
    int runningTime;
    int priority;
} Process;

PCB *createPCB(Process newProcess)
{
    PCB *newPCB = (PCB *)malloc(sizeof(PCB));
    if (newPCB == NULL)
    {
        perror("Error allocating memory for PCB");
        exit(EXIT_FAILURE);
    }
    newPCB->next = NULL;
    newPCB->pid = 0;
    newPCB->id = newProcess.id;
    newPCB->arrivalTime = newProcess.arrivalTime;
    newPCB->runningTime = newProcess.runningTime;
    newPCB->priority = newProcess.priority;
    newPCB->waitingTime = 0;
    newPCB->executionTime = 0;
    newPCB->remainingTime = newProcess.runningTime;
    newPCB->startTime = 0;
    newPCB->preemptedAt = 0;
    newPCB->finishTime = 0;
    newPCB->turnaround = 0;
    newPCB->weightedTurnaround = 0.0;
    newPCB->state = READY;

    return newPCB;
}

typedef struct processMsg
{
    long mtype;
    Process newProcess;
} processMsg;

enum ProcessStates
{
    STARTED = 1,
    RESUMED,
    STOPPED,
    FINISHED
};

enum SchedulingAlgorithm
{
    HPF = 1,
    SRTN,
    RR
};

int *shmaddr;

int getTime()
{
    return *shmaddr;
}

void connectToClk()
{
    int shmid = shmget(SHKEY, 4, 0444);

    while ((int)shmid == -1)
    {
        // Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }

    shmaddr = (int *)shmat(shmid, (void *)0, 0);
}

void disconnectClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}

void sleep_ms(unsigned int milliseconds)
{
    struct timespec req;
    req.tv_sec = milliseconds / 1000;
    req.tv_nsec = (milliseconds % 1000) * 1000000L;
    nanosleep(&req, NULL);
}