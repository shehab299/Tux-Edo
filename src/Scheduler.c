#include "Includes/defs.h"
#include "ReadyQueue.h"
#include "Includes/IO.h"

int msgQueueID;
PCB *running;
int waitingForSignal = 1;

struct IO* output;

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

float getAvgTA(Scheduler *scheduler);
float getAvgWTA(Scheduler *scheduler);
float getCpuUtilization(Scheduler *scheduler);
float getAvgWaiting(Scheduler *scheduler);

void SRTNScheduler(Scheduler *scheduler);
void RRScheduler(Scheduler *scheduler, int timeSlice);
void HPFSchedule(Scheduler *scheduler);

void addProcessToReady(Scheduler* s,Process p);
void outputSummary(Scheduler* s);

void resumeProcess(PCB* process);
void startProcess(PCB* process);
void stopProcess(PCB* process);
void logEvent();


// SIGNAL HANDLERS //

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

        // printf("Scheduler: Message received! with process id %d at time %d\n", msg.newProcess.id, getTime());
        
        addProcessToReady(scheduler,msg.newProcess);
    }

    waitingForSignal = 0;
    signal(SIGUSR1, recieveProcess);
}

void clearResources(int signum)
{
    outputSummary(scheduler);
    free(scheduler);
    exit(0);
}

void terminateProcess(int signum)
{
    running->state = FINISHED;
    running->finishTime = getTime();
    running->responseTime = running->arrivalTime - running->startTime;
    running->turnaround = running->finishTime - running->arrivalTime;
    running->weightedTurnaround = running->waitingTime / running->runningTime;

    scheduler->totalTA += running->turnaround;
    scheduler->totalWTA += running->weightedTurnaround;
    scheduler->totalWaiting += running->waitingTime;

    printLog(output, running, getTime());
    printf("(Scheduler): Process %d started at: %d, finished at: %d\n",
           running->id, running->startTime, running->finishTime);
    signal(SIGUSR2, terminateProcess);
}

int main(int argc, char *argv[])
{
    output = createIO();

    signal(SIGUSR1, recieveProcess);
    signal(SIGUSR2, terminateProcess);
    signal(SIGINT, clearResources);

    msgQueueID = createMessageQueue();
    connectToClk();

    int selectedAlgo = atoi(argv[1]);
    scheduler = createScheduler(selectedAlgo);

    Process initProcess = {.id = -1, .arrivalTime = 0, .priority = 0, .runningTime = 0};

    running = createPCB(initProcess);
    running->state = STOPPED;

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
        int timeSlice = atoi(argv[2]);
        RRScheduler(scheduler, timeSlice);
    }

    disconnectClk(true);
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

        while (waitingForSignal);

        waitingForSignal = 1;

        if (running->state != STARTED && !empty(scheduler->readyQueue))
        {
            running = peek(scheduler->readyQueue);
            dequeue(scheduler->readyQueue);

            startProcess(running);
            logEvent();
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
    int timer = getTime();

    while (true)
    {
        if (timer == getTime())
            continue;

        while (waitingForSignal);
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
            continue;

        PCB* top = peek(scheduler->readyQueue);

        if (running->state != FINISHED && top->remainingTime < running->remainingTime)
        {
            stopProcess(running);
            logEvent();
        }

        if (running->state != STARTED && running->state != RESUMED)
        {
            running = peek(scheduler->readyQueue);
            dequeue(scheduler->readyQueue);

            if (running->startTime == 0)
                startProcess(running);
            else
                resumeProcess(running);


            logEvent();
        }
    }
}

void RRScheduler(Scheduler *scheduler, int timeSlice)
{
    int timer = getTime();
    int remainingTimeSlice = timeSlice;

    while (1)
    {
        if (timer == getTime())
            continue;

        while (waitingForSignal);
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
                stopProcess(running);
                logEvent();
            }
        }

        if (running->state != STARTED &&
            running->state != RESUMED &&
            !empty(scheduler->readyQueue))
        {
            remainingTimeSlice = timeSlice;
            running = peek(scheduler->readyQueue);
            dequeue(scheduler->readyQueue);

            if (running->startTime == 0)
                startProcess(running);
            else
                resumeProcess(running);

            logEvent();
        }

        if (running->state != STARTED && running->state != RESUMED)
        {
            scheduler->idleTime++;
            printf("idle at %d\n", getTime());
        }
    }
}

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

void addProcessToReady(Scheduler* s,Process p)
{
    PCB *newProcessPCB = createPCB(p);
    enqueue(newProcessPCB, scheduler->readyQueue);
    scheduler->numProcesses++;
}

void outputSummary(Scheduler* s)
{
    int cpu = getCpuUtilization(s);
    int avgWTA = getAvgWTA(s);
    int avgTA = getAvgTA(s);
    int avgWaiting = getAvgWaiting(s);

    printPerf(output, cpu, avgWTA, avgWaiting, 0);
}

void resumeProcess(PCB* process){
    process->state = RESUMED;
    process->waitingTime += getTime() - running->preemptedAt;

    kill(process->pid,SIGCONT);   
}

void startProcess(PCB* process){
    process->state = STARTED;
    process->startTime = getTime();
    process->waitingTime += process->startTime - process->arrivalTime;

    int pid = safe_fork();

    if (pid == 0)
    {
        printf("forkked\n");
        char remainingTimeStr[10];
        sprintf(remainingTimeStr, "%d", process->remainingTime);
        execl("./process.out", "./process.out", remainingTimeStr, NULL);
    }

    process->pid = pid;
}

void stopProcess(PCB* process){
    process->state = STOPPED;
    process->preemptedAt = getTime();
    enqueue(process, scheduler->readyQueue);
    kill(process->pid, SIGSTOP);
}

void logEvent(){
    printLog(output, running, getTime());
    printf("Process %d %s at: %d\n", running->id, state(running->state), getTime());
}
