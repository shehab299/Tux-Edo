#include "Includes/defs.h"
#include "ReadyQueue.h"
#include "Includes/IO.h"
#include <math.h>
#include "BuddySystem.h"
int msgQueueID;
int waitingForSignal = 1;

struct IO *output;

typedef struct Scheduler
{
    ReadyQueue *readyQueue;
    ReadyQueue *trash;
    ReadyQueue *waitingQueue;

    PCB *running;
    int numProcesses;
    int numWaitingProcesses;
    int idleTime;
    float cpuUtilization;
    float avgWTA;
    float avgTA;
    float avgWaiting;
    float totalTA;
    float totalWTA;
    float totalWaiting;
    float squaredDeviation;
    float standardDeviation;
} Scheduler;

Scheduler *scheduler;
BuddySystem *buddySystem;
Scheduler *createScheduler(int schedulingAlgo)
{
    Scheduler *scheduler = (Scheduler *)malloc(sizeof(Scheduler));
    scheduler->readyQueue = createReadyQueue(schedulingAlgo);
    scheduler->trash = createReadyQueue(RR);
    scheduler->waitingQueue = createReadyQueue(RR);

    scheduler->numProcesses = 0;
    scheduler->numWaitingProcesses = 0;
    scheduler->idleTime = 0;
    scheduler->cpuUtilization = 0.0;
    scheduler->totalTA = 0.0;
    scheduler->totalWTA = 0.0;
    scheduler->totalWaiting = 0.0;
    scheduler->avgWTA = 0.0;
    scheduler->avgTA = 0.0;
    scheduler->avgWaiting = 0.0;
    scheduler->squaredDeviation = 0.0;
    scheduler->standardDeviation = 0.0;
    return scheduler;
}

int numProcesses;

float getAvgTA(Scheduler *scheduler);
float getAvgWTA(Scheduler *scheduler);
float getCpuUtilization(Scheduler *scheduler);
float getAvgWaiting(Scheduler *scheduler);

void SRTNScheduler(Scheduler *scheduler);
void RRScheduler(Scheduler *scheduler, int timeSlice);
void HPFSchedule(Scheduler *scheduler);

void addProcessToReady(Scheduler *s, PCB *p);
void addProcessToWaiting(Scheduler *s, PCB *p);
void outputSummary(Scheduler *s);

void resumeProcess(PCB *process);
void startProcess(PCB *process);
void stopProcess(PCB *process);
void finishProcess(PCB *process);
void addWaitingToReady(BuddySystem *buddySystem);

void logEvent();

void terminateProcess(int signum);
void clearResources(int signum);
void recieveProcess(int signum);

int main(int argc, char *argv[])
{
    output = createIO();

    signal(SIGUSR1, recieveProcess);
    signal(SIGUSR2, terminateProcess);
    signal(SIGINT, clearResources);
    buddySystem = createBuddySystem(MEMORYSIZE);

    msgQueueID = createMessageQueue(PROCESS_MESSAGE);
    connectToClk();

    int selectedAlgo = atoi(argv[1]);
    scheduler = createScheduler(selectedAlgo);

    Process initProcess = {.id = -1, .arrivalTime = 0, .priority = 0, .runningTime = 0};

    scheduler->running = createPCB(initProcess);
    scheduler->running->state = STOPPED;
    numProcesses = atoi(argv[2]);

    if (selectedAlgo == HPF)
    {
        HPFSchedule(scheduler);
    }
    else if (selectedAlgo == SRTN)
    {
        SRTNScheduler(scheduler);
    }
    else
    {
        int timeSlice = atoi(argv[3]);
        RRScheduler(scheduler, timeSlice);
    }
}

void HPFSchedule(Scheduler *scheduler)
{
    int timer = getTime();

    while (true)
    {
        if (timer == getTime())
        {
            continue;
        }

        while (waitingForSignal)
            ;

        waitingForSignal = 1;

        if (scheduler->running->state != STARTED && !empty(scheduler->readyQueue))
        {
            scheduler->running = peek(scheduler->readyQueue);
            dequeue(scheduler->readyQueue);

            startProcess(scheduler->running);
            logEvent();
        }

        if (scheduler->running->state != STARTED)
        {
            if (numProcesses == 0)
                break;
            scheduler->idleTime++;
            printf("idle at %d\n", getTime());
        }

        scheduler->running->remainingTime--;
    }
    kill(getpid(), SIGINT);
}

void SRTNScheduler(Scheduler *scheduler)
{
    int timer = getTime();
    int process_msgQueueID = createMessageQueue(TIME_MESSAGE);

    while (true)
    {
        if (timer == getTime())
            continue;

        while (waitingForSignal)
            ;
        waitingForSignal = 1;

        timer++;

        if (empty(scheduler->readyQueue) &&
            scheduler->running->state != STARTED &&
            scheduler->running->state != RESUMED)
        {
            if (numProcesses == 0)
                break;
            scheduler->idleTime++;
            printf("idle at %d\n", getTime());
        }

        if (scheduler->running->state == STARTED || scheduler->running->state == RESUMED)
        {
            scheduler->running->remainingTime--;
        }

        if (empty(scheduler->readyQueue))
            continue;

        PCB *top = peek(scheduler->readyQueue);

        if (scheduler->running->state != FINISHED && top->remainingTime < scheduler->running->remainingTime)
        {
            stopProcess(scheduler->running);
            logEvent();
        }

        if (scheduler->running->state != STARTED && scheduler->running->state != RESUMED)
        {
            scheduler->running = peek(scheduler->readyQueue);
            dequeue(scheduler->readyQueue);

            if (scheduler->running->startTime == 0)
                startProcess(scheduler->running);
            else
            {
                timeMsg m = {.mtype = TIME_PROCESS_TYPE, .time = timer};
                msgsnd(process_msgQueueID, &m, sizeof(int), IPC_NOWAIT);
                resumeProcess(scheduler->running);
            }

            logEvent();
        }
    }
    kill(getpid(), SIGINT);
}

void RRScheduler(Scheduler *scheduler, int timeSlice)
{
    int timer = getTime();
    int remainingTimeSlice = timeSlice;
    int process_msgQueueID = createMessageQueue(TIME_MESSAGE);

    while (1)
    {
        if (timer == getTime())
            continue;

        while (waitingForSignal)
            ;
        waitingForSignal = 1;

        timer++;
        addWaitingToReady(buddySystem);
        if (remainingTimeSlice != 0 &&
            (scheduler->running->state == STARTED ||
             scheduler->running->state == RESUMED))
        {
            remainingTimeSlice--;
            scheduler->running->remainingTime -= 1;
        }

        if (remainingTimeSlice == 0 && scheduler->running->state != FINISHED)
        {
            remainingTimeSlice = timeSlice;

            if (!empty(scheduler->readyQueue) && size(scheduler->readyQueue) != 1)
            {
                dequeue(scheduler->readyQueue);
                stopProcess(scheduler->running);
                logEvent();
            }
        }

        if (scheduler->running->state != STARTED &&
            scheduler->running->state != RESUMED &&
            !empty(scheduler->readyQueue))
        {
            remainingTimeSlice = timeSlice;
            scheduler->running = peek(scheduler->readyQueue);

            if (scheduler->running->startTime == 0)
                startProcess(scheduler->running);
            else
            {
                timeMsg m = {.mtype = TIME_PROCESS_TYPE, .time = timer};
                msgsnd(process_msgQueueID, &m, sizeof(int), IPC_NOWAIT);
                resumeProcess(scheduler->running);
            }

            logEvent();
        }

        if (scheduler->running->state != STARTED && scheduler->running->state != RESUMED)
        {
            if (numProcesses == 0)
                break;
            scheduler->idleTime++;
            printf("idle at %d\n", getTime());
        }
    }
    kill(getpid(), SIGINT);
}

float getAvgTA(Scheduler *scheduler)
{
    scheduler->avgTA = (float)scheduler->totalTA / (float)scheduler->numProcesses;
    return scheduler->avgTA;
}

float getAvgWTA(Scheduler *scheduler)
{
    scheduler->avgWTA = (float)scheduler->totalWTA / (float)scheduler->numProcesses;
    return scheduler->avgWTA;
}

float getCpuUtilization(Scheduler *scheduler)
{
    return ((float)(getTime() - scheduler->idleTime) / (float)getTime()) * 100;
}

float getAvgWaiting(Scheduler *scheduler)
{
    scheduler->avgWaiting = (float)scheduler->totalWaiting / (float)scheduler->numProcesses;
    return scheduler->avgWaiting;
}

float getSTD(Scheduler *scheduler, int numProcesses)
{

    PCB *current;
    float avgWTA = getAvgWTA(scheduler);
    float sum = 0;

    while (!empty(scheduler->trash))
    {
        current = peek(scheduler->trash);
        dequeue(scheduler->trash);
        sum += pow((current->weightedTurnaround - avgWTA), 2);
        free(current);
    }

    sum /= numProcesses;

    return sqrt(sum);
}

void addProcessToReady(Scheduler *s, PCB *p)
{
    enqueue(p, scheduler->readyQueue);
    scheduler->numProcesses++;
}
void addProcessToWaiting(Scheduler *s, PCB *p)
{
    enqueue(p, s->waitingQueue);
    scheduler->numWaitingProcesses++;
}

void outputSummary(Scheduler *s)
{
    float cpu = getCpuUtilization(s);
    float avgWTA = getAvgWTA(s);
    float avgTA = getAvgTA(s);
    float avgWaiting = getAvgWaiting(s);
    float std = getSTD(s, s->numProcesses);

    printPerf(output, cpu, avgWTA, avgWaiting, std);
}

void resumeProcess(PCB *process)
{
    process->state = RESUMED;
    process->waitingTime += getTime() - scheduler->running->preemptedAt;

    kill(process->pid, SIGUSR1);
    kill(process->pid, SIGCONT);
}

void startProcess(PCB *process)
{
    process->state = STARTED;
    process->startTime = getTime();
    process->waitingTime += process->startTime - process->arrivalTime;

    int pid = safe_fork();

    if (pid == 0)
    {
        char remainingTimeStr[10];
        sprintf(remainingTimeStr, "%d", process->remainingTime);
        execl("./process.out", "./process.out", remainingTimeStr, NULL);
    }

    process->pid = pid;
}

void stopProcess(PCB *process)
{
    process->state = STOPPED;
    process->preemptedAt = getTime();
    enqueue(process, scheduler->readyQueue);
    kill(process->pid, SIGSTOP);
}

void finishProcess(PCB *process)
{
    process->state = FINISHED;
    process->finishTime = getTime();
    process->remainingTime = 0;
    process->responseTime = process->startTime - process->arrivalTime;
    process->turnaround = process->finishTime - process->arrivalTime;
    process->weightedTurnaround = (float)process->turnaround / (float)process->runningTime;
}

void logEvent()
{
    printLog(output, scheduler->running, getTime());
    printf("Process %d %s at: %d\n", scheduler->running->id, state(scheduler->running->state), getTime());
}

void recieveProcess(int signum)
{
    processMsg msg;

    while (true)
    {
        int rec_val = msgrcv(msgQueueID, &msg, sizeof(msg.newProcess), SCHEDULER_TYPE, IPC_NOWAIT);
        if (rec_val == -1 && errno == ENOMSG)
        {
            break;
        }
        else if (rec_val == -1 && errno != ENOMSG)
        {
            perror("Scheduler: Failed receiving from message queue\n");
            break;
        }

        PCB *pcb = createPCB(msg.newProcess);

        addProcessToWaiting(scheduler, pcb);
    }

    waitingForSignal = 0;
    signal(SIGUSR1, recieveProcess);
}

void clearResources(int signum)
{
    outputSummary(scheduler);
    free(scheduler);
    disconnectClk(true);
    kill(getppid(), SIGINT);
    exit(0);
}

void terminateProcess(int signum)
{
    PCB *pcb = peek(scheduler->readyQueue);
    dequeue(scheduler->readyQueue);
    finishProcess(scheduler->running);
    enqueue(scheduler->running, scheduler->trash);
    deallocateProcess(buddySystem, scheduler->running, scheduler->running->memsize);
    pcb->allocationState = FREED;
    scheduler->totalTA += scheduler->running->turnaround;
    scheduler->totalWTA += scheduler->running->weightedTurnaround;
    scheduler->totalWaiting += scheduler->running->waitingTime;

    logEvent();
    numProcesses--;
    signal(SIGUSR2, terminateProcess);
}

void addWaitingToReady(BuddySystem *buddySystem)
{
    while (scheduler->numWaitingProcesses > 0)
    {
        PCB *pcb = peek(scheduler->waitingQueue);
        // printf("Try to allocate Process = %d   \n", pcb->id);
        // printf("Number in waiting = %d  number in ready = %d\n",size(scheduler->waitingQueue),size(scheduler->readyQueue));
        int startLocation = -1, endLocation = -1;
        if (allocateProcess(buddySystem, pcb, pcb->memsize,&startLocation,&endLocation))
        {
            pcb->startLocation = startLocation;
            pcb->endLocation = endLocation;
            pcb->allocationState = ALLOCATED;
            dequeue(scheduler->waitingQueue);
            addProcessToReady(scheduler, pcb);
            scheduler->numWaitingProcesses--;
        }
        else
            break;
    }
}
