#include "Includes/defs.h"
#include "ReadyQueue.h"
#include "Includes/OutputFile.h"

int msgQueueID;
PCB *running;
PCB *top;
int waitingForSignal = 1;

FILE *outputFile;
FILE *perfFile;

typedef struct Scheduler
{
    ReadyQueue *readyQueue;
    int numProcesses;
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

Scheduler *createScheduler(int schedulingAlgo)
{
    Scheduler *scheduler = (Scheduler *)malloc(sizeof(Scheduler));
    scheduler->readyQueue = createReadyQueue(schedulingAlgo);
    scheduler->numProcesses = 0;
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

// CALCULATIONS //

float getAvgTA(Scheduler *scheduler)
{
    scheduler->avgTA = scheduler->avgTA / scheduler->numProcesses;
    return scheduler->avgTA;
}

float getAvgWTA(Scheduler *scheduler)
{
    scheduler->avgWTA = scheduler->avgWTA / scheduler->numProcesses;
    return scheduler->avgWTA;
}

float getCpuUtilization(Scheduler *scheduler)
{
    return ((getTime() - scheduler->idleTime) / getTime()) * 100;
}

float getAvgWaiting(Scheduler *scheduler)
{
    scheduler->avgWaiting = scheduler->totalWaiting / scheduler->numProcesses;
    return scheduler->avgWaiting;
}

// SIGNAL HANDLERS //

void processMessageReceiver(int signum)
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
        // printf("Scheduler: Message received! with process id %d at time %d\n", msg.newProcess.id, getTime());
        PCB *newProcessPCB = createPCB(msg.newProcess);
        enqueue(newProcessPCB, scheduler->readyQueue);
        scheduler->numProcesses++;
    }
    waitingForSignal = 0;
    signal(SIGUSR1, processMessageReceiver);
}

void clearResources(int signum)
{
    int cpu = getCpuUtilization(scheduler);
    int avgWTA = getAvgWTA(scheduler);
    int avgTA = getAvgTA(scheduler);

    printPerf(perfFile, cpu, avgWTA, 0, 0);
    exit(0);
    signal(SIGINT, clearResources);
}

void processTermination(int signum)
{
    running->state = FINISHED;
    running->finishTime = getTime();
    running->responseTime = running->arrivalTime - running->startTime;
    running->turnaround = running->finishTime - running->arrivalTime;
    running->weightedTurnaround = running->waitingTime / running->runningTime;

    scheduler->totalTA += running->turnaround;
    scheduler->totalWTA += running->weightedTurnaround;
    scheduler->totalWaiting += running->waitingTime;

    printLog(outputFile, running, getTime());
    printf("(Scheduler): Process %d started at: %d, finished at: %d\n",
           running->id, running->startTime, running->finishTime);
    signal(SIGUSR2, processTermination);
}

// SCHEDULING ALGORITHMS //

void HPFScheduler(Scheduler *scheduler)
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

        if (running->state != STARTED && !empty(scheduler->readyQueue))
        {
            running = peek(scheduler->readyQueue);
            running->state = STARTED;
            running->startTime = getTime();
            running->waitingTime = getTime() - running->arrivalTime;

            dequeue(scheduler->readyQueue);

            printLog(outputFile, running, getTime());
            printf("Process %d %s at: %d\n", running->id, state(running->state), getTime());

            int pid = safe_fork();

            if (pid == 0)
            {
                char runningTimeStr[10];
                sprintf(runningTimeStr, "%d", running->runningTime);
                execl("./process.out", "./process", runningTimeStr, NULL);
            }
        }

        if (running->state != STARTED)
        {
            scheduler->idleTime++;
            printf("idle at %d\n", getTime());
        }
    }
}

void SRTNScheduler(Scheduler *scheduler)
{
    int pid;
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

        running->remainingTime -= 1;
        timer++;

        if (empty(scheduler->readyQueue) &&
            running->state != STARTED &&
            running->state != RESUMED)
        {
            scheduler->idleTime++;
            printf("idle at %d\n", getTime());
        }

        if (empty(scheduler->readyQueue))
        {
            continue;
        }

        top = peek(scheduler->readyQueue);

        if (running->state != FINISHED && top->remainingTime < running->remainingTime)
        {
            running->state = STOPPED;
            running->preemptedAt = getTime();
            printLog(outputFile, running, getTime()); // logging
            printf("process %d %s at time %d, remainig time = %d\n",
                   running->id, state(running->state),
                   running->remainingTime, getTime());
            enqueue(running, scheduler->readyQueue);
            kill(pid, SIGINT);
        }

        if (running->state != STARTED && running->state != RESUMED)
        {
            running = peek(scheduler->readyQueue);
            dequeue(scheduler->readyQueue);

            if (running->startTime == 0)
            {
                running->state = STARTED;
                running->startTime = getTime();
                running->waitingTime += running->startTime - running->arrivalTime;
            }
            else
            {
                running->state = RESUMED;
                running->waitingTime += getTime() - running->preemptedAt;
            }

            pid = safe_fork();

            if (pid == 0)
            {
                char remainingTimeStr[10];
                sprintf(remainingTimeStr, "%d", running->remainingTime);
                printf("Process %d %s at: %d\n", running->id,
                       state(running->state), getTime());
                execl("./process.out", "./process.out", remainingTimeStr, NULL);
            }
            printLog(outputFile, running, getTime()); // logging
        }
    }
}

void RRScheduler(Scheduler *scheduler, int timeSlice)
{
    int pid;
    int timer = getTime();
    int remainingTimeSlice = timeSlice;
    while (1)
    {
        if (timer == getTime())
        {
            continue;
        }

        while (waitingForSignal)
            ;

        waitingForSignal = 1;

        timer++;
        if (remainingTimeSlice != 0 &&
            (running->state == STARTED ||
             running->state == RESUMED))
        {
            remainingTimeSlice--;
        }

        if (remainingTimeSlice == 0 && running->state != FINISHED)
        {
            running->remainingTime -= timeSlice;
            remainingTimeSlice = timeSlice;
            if (!empty(scheduler->readyQueue))
            {
                running->state = STOPPED;
                running->preemptedAt = getTime();

                kill(pid, SIGINT);
                enqueue(running, scheduler->readyQueue);
                printLog(outputFile, running, getTime());
                printf("process %d %s at time %d, remainig time = %d\n",
                       running->id, state(running->state),
                       running->remainingTime, getTime());
            }
        }

        if (running->state != STARTED &&
            running->state != RESUMED &&
            !empty(scheduler->readyQueue))
        {
            remainingTimeSlice = timeSlice;
            running = peek(scheduler->readyQueue);
            dequeue(scheduler->readyQueue);

            pid = safe_fork();

            if (running->startTime == 0)
            {
                running->state = STARTED;
                running->startTime = getTime();
                running->waitingTime += running->startTime - running->arrivalTime;
            }
            else
            {
                running->state = RESUMED;
                running->waitingTime += getTime() - running->preemptedAt;
            }

            if (pid == 0)
            {
                char remainingTimeStr[10];
                sprintf(remainingTimeStr, "%d", running->remainingTime);
                printf("Process %d %s at: %d\n", running->id, state(running->state), getTime());

                execl("./process.out", "./process.out", remainingTimeStr, NULL);
            }
            printLog(outputFile, running, getTime());
        }

        if (running->state != STARTED && running->state != RESUMED)
        {
            scheduler->idleTime++;
            printf("idle at %d\n", getTime());
        }
    }
}

int main(int argc, char *argv[])
{
    intializeLogFile(&outputFile);
    intializePerfFile(&perfFile);

    signal(SIGUSR1, processMessageReceiver);
    signal(SIGUSR2, processTermination);
    signal(SIGINT, clearResources);

    msgQueueID = createMessageQueue();
    connectToClk();

    int selectedAlgo = atoi(argv[1]);
    scheduler = createScheduler(selectedAlgo);

    Process initProcess = {.id = -1, .arrivalTime = 0, .priority = 0, .runningTime = 0};

    top = createPCB(initProcess);
    running = createPCB(initProcess);
    running->state = STOPPED;

    if (selectedAlgo == HPF)
    {
        HPFScheduler(scheduler);
    }
    else if (selectedAlgo == SRTN)
    {
        SRTNScheduler(scheduler);
    }
    else
    {
        int timeSlice = atoi(argv[2]);
        RRScheduler(scheduler, timeSlice);
    }

    disconnectClk(true);
}