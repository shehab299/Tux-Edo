#pragma once

#include <stdio.h>      //if you don't use scanf/printf change this include
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

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300

typedef struct PCB
{
  struct PCB *next;
  int pid;         // id from forking
  int id;          // id from input file
  int arrivalTime; // the time the process arrived at
  int runningTime; // total time needed by the process to finish executing
  int priority;
  int waitingTime;   // total time the process spend at readu and block queue
  int executionTime; //
  int remainingTime;
  int startingTime;
  int preemptedAt; // last Time the process preempted at
  int finishTime;
  int turnaround;
  float weightedTurnaround;
  int state; // 10->started    11->resumed    12->stopped    13->finished   14->running
} PCB;

typedef struct Process
{
    int id;
    int arrivalTime;
    int runningTime;
    int priority;
} Process;

enum ProcessStates{
	STARTED = 1,
	RESUMED,
	STOPPED,
	FINISHED
};

int* shmaddr; //No Need To Change It.


int getTime()
{
    return *shmaddr;
}

void connectToClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }

    shmaddr = (int *) shmat(shmid, (void *)0, 0);
}


void disconnectClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}
