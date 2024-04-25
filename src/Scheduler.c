#include "Includes/defs.h"
#include "./DataStructures/priorityQ.h"

int rec_val, msgQueueID;
processMsg msg;
int currentTime;
PCB *running;
PCB *top;

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


typedef struct Scheduler
{
    Heap *readyQueue;
    int numProcesses;
    int cpuUtilization;
    // int currentTime;
    float avgWTA;
    float avgTA;
    float standardDeviation;
    float totalTA;
    float totalWTA;
} Scheduler;

Scheduler *scheduler;

bool comparePriority(void *a, void *b)
{
    PCB *processA = (PCB *)a;
    PCB *processB = (PCB *)b;
    return processA->priority < processB->priority;
}

bool compareRemainingTime(void *a, void *b)
{
    PCB *processA = (PCB *)a;
    PCB *processB = (PCB *)b;
    return processA->remainingTime < processB->remainingTime;
}

Scheduler *createScheduler(int schedulingAlgo)
{
    Scheduler *scheduler = (Scheduler *)malloc(sizeof(Scheduler));
    if (schedulingAlgo == HPF)
    {
        scheduler->readyQueue = create_heap(10, comparePriority);
    }
    else if (schedulingAlgo == SRTN)
    {
        scheduler->readyQueue = create_heap(10, compareRemainingTime);
    }
    // scheduler->currentTime = getTime();
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
    running->state = TERMINATED;
    running->finishTime = getTime();
    running->waitingTime = running->arrivalTime - running->startTime;
    running->turnaround = running->finishTime - running->arrivalTime;
    running->weightedTurnaround = running->waitingTime / running->runningTime;
    scheduler->totalTA += running->turnaround;
    scheduler->totalWTA += running->weightedTurnaround;
    printf("(Scheduler): Prcoss %d terminated! started at: %d, finished at: %d\n", running->id, running->startTime, running->finishTime);
    // free(running);
    signal(SIGUSR2, processTermination);
}

void processMessageReceiver(int signum)
{
    while (true)
    {
        rec_val = msgrcv(msgQueueID, &msg, sizeof(msg.newProcess), getpid() % 10000, IPC_NOWAIT);
        if (rec_val == -1 && errno == ENOMSG)
        {
            // printf("Scheduler: Message queue is empty\n");
            break;
        }
        else if (rec_val == -1 && errno != ENOMSG)
        {
            perror("Scheduler:Failed receiving from message queue\n");
            break;
        }

        PCB *newProcessPCB = createPCB(msg.newProcess);
        insert((void *)newProcessPCB, scheduler->readyQueue);
        scheduler->numProcesses++;
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
        running->startTime = getTime();
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

void SRTNScheduler(Scheduler *scheduler)
{
    int pid;
    if (running->state != RUNNING && !isEmpty(scheduler->readyQueue))
    {
        running = (PCB *)minElement((void *)scheduler->readyQueue);
        running->state = RUNNING;
        deleteMin(scheduler->readyQueue);
        if (running->startTime == 0)
        {
            running->startTime = getTime();
        }

        pid = fork();
        if (pid == -1)
        {
            perror("Error in forking process\n");
            exit(-1);
        }
        else if (pid == 0)
        {
            char remainingTimeStr[10];
            sprintf(remainingTimeStr, "%d", running->remainingTime);

            printf("Process %d proceeded at: %d\n", running->id, getTime());
            execl("/home/asmaa/Desktop/Tux-Edo/process.out", "./process", remainingTimeStr, NULL);
        }
    }

    if (currentTime != getTime())
    {
        running->remainingTime -= getTime() - currentTime;
        // printf("NOT EQUAL!!!!!\n");
        currentTime = getTime();
    }

    if (!isEmpty(scheduler->readyQueue))
    {
        top = (PCB *)minElement((void *)scheduler->readyQueue);
        // printf("Process %d remaining time = %d at time = %d\n", running->id, running->remainingTime, getTime());
        // printf("Top process id = %d remaining Time = %d at time = %d\n", top->id, top->remainingTime, getTime());

        if (running->state != TERMINATED && top->remainingTime < running->remainingTime)
        {
            running->state = READY;
            printf("Switch process %d, remainig time = %d\n", running->id, running->remainingTime);
            insert((void *)running, scheduler->readyQueue);
            kill(pid, SIGINT);
        }
    }
}

int main(int argc, char *argv[])
{
    int selectedAlgo = atoi(argv[1]);
    // printf("Selected algorithm: %d\n", selectedAlgo);
    scheduler = createScheduler(selectedAlgo);
    signal(SIGUSR1, processMessageReceiver);
    signal(SIGUSR2, processTermination);
    signal(SIGINT, clearResources);
    // printf("Hello from scheduler!\n");

    connectToClk();

    msgQueueID = createMessageQueue();

    msg.mtype = getpid() % 10000;
    Process initProcess = {.id=-1,.arrivalTime=0,.priority=0,.runningTime=0};
    running = createPCB(initProcess);
    top = createPCB(initProcess);
    running->state = READY;

    currentTime = getTime();

    while (1)
    {
        if (selectedAlgo == HPF)
            HPFScheduler(scheduler);
        else if (selectedAlgo == SRTN)
        {
            SRTNScheduler(scheduler);
        }

        // else
        // {
        //     RR(scheduler);
        // }
    }
    disconnectClk(true);
}

