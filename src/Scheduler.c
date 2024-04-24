#include "Includes/defs.h"
#include "./DataStructures/priorityQ.h"

#define READY 1
#define RUNNING 2
#define TERMINATED 3

int rec_val, msgQueueID;
processMsg msg;
PCB *running;

typedef struct Scheduler
{
    Heap *readyQueue;
    int numProcesses;
    int cpuUtilization;
    float avgWTA;
    float avgTA;
    float standardDeviation;
    float totalTA;
    float totalWTA;
} Scheduler;

Scheduler *scheduler;

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
    newPCB->remainingTime = newPCB->runningTime;
    newPCB->startingTime = 0;
    newPCB->preemptedAt = 0;
    newPCB->finishTime = 0;
    newPCB->turnaround = 0;
    newPCB->weightedTurnaround = 0.0;
    newPCB->state = READY;

    return newPCB;
}

bool comparePriority(void *a, void *b)
{
    PCB *processA = (PCB *)a;
    PCB *processB = (PCB *)b;
    return processA->priority < processB->priority;
}

Scheduler *createScheduler()
{
    Scheduler *scheduler = (Scheduler *)malloc(sizeof(Scheduler));
    scheduler->readyQueue = create_heap(10, comparePriority);
    scheduler->numProcesses = 0;
    scheduler->cpuUtilization = 0.0;
    scheduler->totalTA = 0.0;
    scheduler->totalWTA = 0.0;
    scheduler->avgWTA = 0.0;
    scheduler->avgTA = 0.0;
    scheduler->standardDeviation = 0.0;
    return scheduler;
}

void processTermination(int signum)
{
    printf("signal  termination Received\n");
    running->state = TERMINATED;
    running->finishTime = getTime();
    running->turnaround = running->finishTime - running->arrivalTime;
    running->weightedTurnaround = running->waitingTime / running->runningTime;
    scheduler->totalTA += running->turnaround;
    scheduler->totalWTA += running->weightedTurnaround;
    printf("(Scheduler): Prcoss %d terminated! started at: %d, finished at: %d\n", running->id, running->startingTime, running->finishTime);

    signal(SIGUSR2, processTermination);
}

void processMessageReceiver(int signum)
{
    while (true)
    {
        rec_val = msgrcv(msgQueueID, &msg, sizeof(msg.newProcess), getpid() % 10000, IPC_NOWAIT);
        if (rec_val == -1 && errno == ENOMSG)
        {
            printf("Scheduler: Message queue is empty\n");
            break;
        }
        else if (rec_val == -1 && errno != ENOMSG)
        {
            perror("Scheduler:Failed receiving from message queue\n");
            break;
        }

        printf("Scheduler: Message received! with process id %d\n", msg.newProcess.id);
        PCB *newProcessPCB = createPCB(msg.newProcess);
        // printf("Scheduler: PCB is created successfully: %d\n", newProcessPCB->arrivalTime);

        insert((void *)newProcessPCB, scheduler->readyQueue);
        printf("Num of processes in ready: %d\n", getCount(scheduler->readyQueue));
        scheduler->numProcesses++;
        printf("Scheduler: num of processes are now: %d\n", scheduler->numProcesses);
    }
    signal(SIGUSR1, processMessageReceiver);
}

void clearResources(int signum)
{
    exit(0);
    signal(SIGINT, clearResources);
}

void HPFScheduler(Scheduler *scheduler)
{
    if (running->state != RUNNING && !isEmpty(scheduler->readyQueue))
    {
        running = (PCB *)minElement((void *)scheduler->readyQueue);
        running->state = RUNNING;
        running->startingTime = getTime();
        deleteMin(scheduler->readyQueue);
        printf("Scheduler: Running process with pid = %d, runningTime = %d, Priority: %d\n", running->id, running->runningTime, running->priority);
        int pid = fork();

        if (pid == -1)
        {
            perror("Error in forking process\n");
            exit(-1);
        }
        else if (pid == 0)
        {
            char runningTimeStr[10];
            sprintf(runningTimeStr, "%d", running->runningTime);
            execl("/home/asmaa/Desktop/Tux-Edo/process.out", "./process", runningTimeStr, NULL);
        }
    }
}

int main()
{
    scheduler = createScheduler();
    signal(SIGUSR1, processMessageReceiver);
    signal(SIGUSR2, processTermination);
    signal(SIGINT, clearResources);
    printf("Hello from scheduler!\n");

    connectToClk();

    key_t key_id = ftok("../keyfile", 65);
    msgQueueID = msgget(key_id, 0666 | IPC_CREAT);

    if (msgQueueID == -1)
    {
        perror("Error in creating message queue!");
        exit(-1);
    }

    msg.mtype = getpid() % 10000;
    Process initProcess;
    initProcess.id = -1;
    initProcess.arrivalTime = 0;
    initProcess.runningTime = 0;
    initProcess.priority = 0;
    running = createPCB(initProcess);
    running->state = READY;
    while (1)
    {
        HPFScheduler(scheduler);
    }

    // while (getTime() < 35)
    // {
    //     sleep_ms(950);
    //     printf("Scheduler still running\n");
    // }

    disconnectClk(true);
}