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
#include <stdbool.h>
#include <errno.h>
#include <time.h>

#define SHKEY 300
#define SEM1KEY 100
#define SEM2KEY 200

typedef struct Process
{
    int id;
    int arrivalTime;
    int runningTime;
    int priority;
} Process;

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
    enum ProcessStates state;
} PCB;

enum MessageTypes
{
    SCHEDULER_TYPE = 505,
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

int createMessageQueue()
{
    key_t key_id = ftok("../keyfile", 65);
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

    return newPCB;
}
