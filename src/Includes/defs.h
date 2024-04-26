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

extern int redirectOutput()
{
    FILE *output_file;
    output_file = fopen("output.txt", "w");

    if (output_file == NULL) {
        fprintf(stderr, "Error opening file.\n");
        return 1; 
    }

    freopen("output.txt", "w", stdout);
    return 0;
}


typedef struct Process
{
    int id;
    int arrivalTime;
    int runningTime;
    int priority;
} Process;

extern PCB *createPCB(Process newProcess)
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

enum MessageTypes{
    SCHEDULER_TYPE = 505,
};

int *shmaddr;

extern int getTime()
{
    return *shmaddr;
}

extern void connectToClk()
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

extern void disconnectClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}

extern void sleep_ms(unsigned int milliseconds)
{
    struct timespec req;
    req.tv_sec = milliseconds / 1000;
    req.tv_nsec = (milliseconds % 1000) * 1000000L;
    nanosleep(&req, NULL);
}

union Semun
{
    int val;        
    struct semid_ds *buf;  
    unsigned short *array; 
    struct seminfo *__buf; 
};

extern void down(int sem)
{
    struct sembuf op;

    op.sem_num = 0;
    op.sem_op = -1;
    op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &op, 1) == -1)
    {
        perror("Error in down()");
        exit(-1);
    }
}

extern void up(int sem)
{
    struct sembuf op;

    op.sem_num = 0;
    op.sem_op = 1;
    op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &op, 1) == -1)
    {
        perror("Error in up()");
        exit(-1);
    }
}



extern int* createSemaphore(int key)
{
    union Semun semun;

    int* sem = malloc(sizeof(int));
    *sem = semget(key, 1, 0666 | IPC_CREAT);

    if(*sem == -1)
    {
        perror("Error in create sem");
        exit(-1);
    }

    semun.val = 0;

    if (semctl(*sem, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }

    return sem;
}



extern int safe_fork()
{
    int pid = fork();

    if(pid == -1)
    {
        perror("Error in forking");
        exit(-1);
    }

    return pid;
}

extern FILE* safe_fopen(const char* path, const char* perms){

    FILE* fptr = fopen(path,perms);

    if(fptr == NULL)
    {
        perror("Couldn't Open Seed File");
        exit(-1);   
    }

    return fptr;
}
