#include "Includes/defs.h"
#include "ReadyQueue.h"

int rec_val, msgQueueID;
processMsg msg;
PCB *running;
PCB *top;
int waitingForSignal = 1;

const char *state(enum ProcessStates state)
{
    switch (state)
    {
    case STARTED:
        return "started";
    case RESUMED:
        return "resumed";
    case STOPPED:
        return "stopped";
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

typedef struct Scheduler
{
    ReadyQueue *readyQueue;
    int numProcesses;
    int cpuUtilization;
    float avgWTA;
    float avgTA;
    float standardDeviation;
    float totalTA;
    float totalWTA;
} Scheduler;

Scheduler *scheduler;

Scheduler *createScheduler(int schedulingAlgo)
{
    Scheduler *scheduler = (Scheduler *)malloc(sizeof(Scheduler));
    scheduler->readyQueue = createReadyQueue(schedulingAlgo);
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
    running->state = FINISHED;
    running->finishTime = getTime();
    running->waitingTime = running->arrivalTime - running->startTime;
    running->turnaround = running->finishTime - running->arrivalTime;
    running->weightedTurnaround = running->waitingTime / running->runningTime;
    scheduler->totalTA += running->turnaround;
    scheduler->totalWTA += running->weightedTurnaround;
    printf("(Scheduler): Process %d started at: %d, finished at: %d\n", running->id, running->startTime, running->finishTime);
    signal(SIGUSR2, processTermination);
}

void processMessageReceiver(int signum)
{
    while (true)
    {
        rec_val = msgrcv(msgQueueID, &msg, sizeof(msg.newProcess), SCHEDULER_TYPE, IPC_NOWAIT);
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
    exit(0);
    signal(SIGINT, clearResources);
}

void HPFScheduler(Scheduler *scheduler)
{

    int timer = getTime();

    while (true)
    {
        if (timer == getTime())
            continue;

        while (waitingForSignal)
            ;
        waitingForSignal = 1;

        if (running->state != STARTED && !empty(scheduler->readyQueue))
        {
            running = peek(scheduler->readyQueue);
            running->state = STARTED;
            running->startTime = getTime();
            dequeue(scheduler->readyQueue);

            printf("Scheduler: Running process with pid = %d, runningTime = %d, Priority: %d\n", running->id, running->runningTime, running->priority);
            int pid = safe_fork();

            if (pid == 0)
            {
                char runningTimeStr[10];
                sprintf(runningTimeStr, "%d", running->runningTime);
                execl("./process.out", "./process", runningTimeStr, NULL);
            }
        }
    }
}

void SRTNScheduler(Scheduler *scheduler)
{
    int timer = getTime();
    int pid;

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

        if (!empty(scheduler->readyQueue))
        {
            top = peek(scheduler->readyQueue);

            if (running->state != FINISHED && top->remainingTime < running->remainingTime)
            {
                running->state = STOPPED;
                printf("process %d %s at time %d, remainig time = %d\n",
                       running->id, state(running->state),
                       running->remainingTime, getTime());
                enqueue(running, scheduler->readyQueue);
                kill(pid, SIGINT);
            }
        }

        if (running->state != STARTED && running->state != RESUMED && !empty(scheduler->readyQueue))
        {
            running = peek(scheduler->readyQueue);
            dequeue(scheduler->readyQueue);

            if (running->startTime == 0)
            {
                running->state = STARTED;
                running->startTime = getTime();
            }
            else
            {
                running->state = RESUMED;
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
                kill(pid, SIGINT);
                enqueue(running, scheduler->readyQueue);
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
            running->state = RUNNING;
            dequeue(scheduler->readyQueue);
            pid = safe_fork();
            if (running->startTime == 0)
            {
                running->state = STARTED;
                running->startTime = getTime();
            }
            else
            {
                running->state = RESUMED;
            }

            if (pid == 0)
            {
                char remainingTimeStr[10];
                sprintf(remainingTimeStr, "%d", running->remainingTime);
                printf("Process %d %s at: %d\n", running->id, state(running->state), getTime());

                execl("./process.out", "./process.out", remainingTimeStr, NULL);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    int timeSlice = 4;

    signal(SIGUSR1, processMessageReceiver);
    signal(SIGUSR2, processTermination);
    signal(SIGINT, clearResources);

    msgQueueID = createMessageQueue();
    connectToClk();

    int selectedAlgo = atoi(argv[1]);
    scheduler = createScheduler(selectedAlgo);

    msg.mtype = SCHEDULER_TYPE;

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
        RRScheduler(scheduler, timeSlice);
    }

    disconnectClk(true);
}