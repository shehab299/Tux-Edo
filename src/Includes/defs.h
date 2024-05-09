#pragma once
#ifndef _defs
#define _defs
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
#include <stdbool.h>
#include <errno.h>
#include <time.h>

#define SHKEY 300
#define SEM1KEY 100
#define SEM2KEY 200
#define MEMORYSIZE  1024

typedef struct Process
{
    int id;
    int arrivalTime;
    int runningTime;
    int priority;
    int memsize;
} Process;

typedef struct processMsg
{
    long mtype;
    Process newProcess;
} processMsg;

typedef struct timeMsg
{
    long mtype;
    int time;
} timeMsg;

enum ProcessStates
{
    STARTED = 1,
    RESUMED,
    STOPPED,
    FINISHED
};

enum AllocationStates
{
    ALLOCATED = 1,
    FREED
};

enum SchedulingAlgorithm
{
    HPF = 1,
    SRTN,
    RR
};

enum MessageQueues
{
    PROCESS_MESSAGE = 65,
    TIME_MESSAGE,
};

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
    int responseTime;
    int preemptedAt;
    int finishTime;
    int turnaround;
    float weightedTurnaround;
    int memsize;
    int startLocation;
    int endLocation;
    enum AllocationStates allocationState;
    enum ProcessStates state;
} PCB;

enum MessageTypes
{
    SCHEDULER_TYPE = 505,
    TIME_PROCESS_TYPE
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

int safe_fork()
{
    int pid = fork();

    if (pid == -1)
    {
        perror("Error in forking");
        exit(-1);
    }

    return pid;
}

FILE *safe_fopen(const char *path, const char *perms)
{

    FILE *fptr = fopen(path, perms);

    if (fptr == NULL)
    {
        perror("Couldn't Open Seed File");
        exit(-1);
    }

    return fptr;
}

const char *state(enum ProcessStates state)
{
    switch (state)
    {
    case STARTED:
        return "started ";
    case RESUMED:
        return "resumed ";
    case STOPPED:
        return "stopped ";
    case FINISHED:
        return "finished";
    default:
        return "Unknown";
    }
}

const char *allocationState(enum AllocationStates state)
{
    switch (state)
    {
    case ALLOCATED:
        return "allocated";
    case FREED:
        return "freed   ";
    default:
        return "Unknown";
    }
}

int createMessageQueue(int proj_id)
{
    key_t key_id = ftok("../keyfile", proj_id);
    int msgQueueID = msgget(key_id, 0666 | IPC_CREAT);

    if (msgQueueID == -1)
    {
        perror("Error in creating message queue!");
        exit(-1);
    }

    return msgQueueID;
}

PCB *createPCB(Process newProcess)
{
    PCB *newPCB = (PCB *)malloc(sizeof(PCB));
    if (newPCB == NULL)
    {
        perror("Error allocating memory for PCB");
        exit(EXIT_FAILURE);
    }
    newPCB->pid = 0;
    newPCB->id = newProcess.id;
    newPCB->arrivalTime = newProcess.arrivalTime;
    newPCB->runningTime = newProcess.runningTime;
    newPCB->priority = newProcess.priority;
    newPCB->waitingTime = 0;
    newPCB->executionTime = 0;
    newPCB->remainingTime = newProcess.runningTime;
    newPCB->startTime = 0;
    newPCB->responseTime = 0;
    newPCB->preemptedAt = 0;
    newPCB->finishTime = 0;
    newPCB->turnaround = 0;
    newPCB->weightedTurnaround = 0.0;
    newPCB->state = STOPPED;
    newPCB->memsize = newProcess.memsize;
    newPCB->startLocation = -1;
    newPCB->endLocation = -1;

    return newPCB;
}
#endif
