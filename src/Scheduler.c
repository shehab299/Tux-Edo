#include "./Includes/defs.h"
#include "DataStructures/priorityQ.h"

#define READY 1
#define RUNNING 2
#define TERMINATED 3

int rec_val, msgQueueID;
processMsg msg;

void clearResources(int signum)
{
    exit(0);
    signal(SIGINT, clearResources);
}

bool comparePriority(void *a, void *b)
{
    PCB *processA = (PCB *)a;
    PCB *processB = (PCB *)b;
    return processA->priority < processB->priority;
}

PCB createPCB(Process newProcess)
{
    PCB newPCB;

    newPCB.next = NULL;
    newPCB.pid = 0;
    newPCB.id = newProcess.id;
    newPCB.arrivalTime = newProcess.arrivalTime;
    newPCB.runningTime = newProcess.runningTime;
    newPCB.priority = newProcess.priority;
    newPCB.waitingTime = 0;
    newPCB.executionTime = 0;
    newPCB.remainingTime = newPCB.runningTime;
    newPCB.startingTime = 0;
    newPCB.preemptedAt = 0;
    newPCB.finishTime = 0;
    newPCB.turnaround = 0;
    newPCB.weightedTurnaround = 0.0;
    newPCB.state = READY;

    return newPCB;
}

typedef struct Scheduler
{
    Heap *readyQueue;
    int numProcesses;
    int cpuUtilization;
    float avgWTA;
    float avgTA;
    int standardDeviation;
    float totalTA;
    float totalWTA;

} Scheduler;

void processMessageReceiver(int signum)
{
    int rec_val = msgrcv(msgQueueID, &msg, sizeof(msg.newProcess),
                         getpid() % 10000, IPC_NOWAIT);
    while (rec_val != -1 || errno != ENOMSG)
    {
        if (errno == ENOMSG)
        {
            printf("Scheduler: Message queue is empty\n");
            break;
        }

        printf("Scheduler: Message received! with process id %d\n", msg.newProcess.id);
        createPCB(msg.newProcess);
        // scheduler->numProcesses++;
    }
    /*     if (rec_val == -1)
            perror("Error in receiving message from ProcessGen!"); */

    signal(SIGUSR1, processMessageReceiver);
}

void processTerminates(int sigNum)
{
    signal(SIGUSR1, processTerminates);
}

float getAvgTA(Scheduler *scheduler)
{
    scheduler->avgTA = scheduler->totalTA / scheduler->numProcesses;
    return scheduler->avgTA;
}

float getAvgWTA(Scheduler *scheduler)
{
    scheduler->avgWTA = scheduler->totalWTA / scheduler->numProcesses;
    return scheduler->avgWTA;
}

void HPFScheduler(Scheduler *scheduler)
{
    // Heap *pq = create_heap(scheduler->numProcesses, true);

    while (1)
    {
        if (!isEmpty(scheduler->readyQueue))
        {
            PCB *running = (PCB *)minElement((void *)scheduler->readyQueue);
            running->state = RUNNING;
            running->startingTime = getTime();
            int pid = fork();

            if (pid == -1)
            {
                perror("Error in forking process");
                exit(-1);
            }
            else if (pid == 0)
            {
                execl("./process", "./process", NULL);
            }
            else
            {
                waitpid(pid, NULL, 0);
                running->state = TERMINATED;
                running->finishTime = getTime();
                running->turnaround = running->finishTime - running->arrivalTime;
                running->weightedTurnaround = running->waitingTime / running->runningTime;
                scheduler->totalTA += running->turnaround;
                scheduler->totalWTA += running->weightedTurnaround;

                deleteMin(scheduler->readyQueue);
            }
        }
    }
}

Scheduler *createScheduler()
{
    Scheduler *scheduler = (Scheduler *)malloc(sizeof(Scheduler));
    Heap *readyQueue = create_heap(scheduler->numProcesses, comparePriority);
    scheduler->numProcesses = 0.0;
    scheduler->cpuUtilization = 0.0;
    scheduler->totalTA = 0.0;
    scheduler->totalWTA = 0.0;
    scheduler->avgWTA = 0.0;
    scheduler->avgTA = 0.0;
    scheduler->standardDeviation = 0.0;
}

int main(int argc, char *argv[])
{
    signal(SIGUSR1, processMessageReceiver);
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

    printf("Scheduler: message queue created with id %d\n", msgQueueID);

    msg.mtype = getpid() % 10000;

    while (getTime() < 35)
    {
        sleep_ms(950);
        printf("Scheduler still running\n");
    }

    disconnectClk(true);
}