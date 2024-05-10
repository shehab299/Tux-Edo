#include "Includes/UI.h"
#include "DataStructures/Queue.h"
#include "Includes/defs.h"

void clearResources(int);
void sendMessageToScheduler(int, Process *);
void initalizeQueue(char *filePath, Queue *queue);

int msgQueueID;
int schedulerId;

int main(int argc, char *argv[])
{
    msgQueueID = createMessageQueue(PROCESS_MESSAGE);

    signal(SIGINT, clearResources);

    if (argc < 2)
    {
        perror("Didn't Provide The Input File");
        exit(-1);
    }

    Queue *pQueue = createQueue();
    initalizeQueue("./processes.txt", pQueue);

    int *selectedAlgo = malloc(sizeof(int));
    int *timeSlice = malloc(sizeof(int));

    userInput(selectedAlgo, timeSlice);

    int clkPid = safe_fork();

    if (clkPid == 0)
        execl("./clk.out", "./clk.out", NULL);

    connectToClk();

    schedulerId = fork();

    if (schedulerId == 0)
    {
        char selectedAlgoStr[10];
        sprintf(selectedAlgoStr, "%d", *selectedAlgo);

        char timeSliceStr[10];
        char numProcessesStr[10];
        sprintf(numProcessesStr, "%d", q_getSize((void *)pQueue));
        if (*selectedAlgo == 3)
        {
            sprintf(timeSliceStr, "%d", *timeSlice);
            execl("./scheduler.out", "./scheduler.out", selectedAlgoStr, numProcessesStr, timeSliceStr, NULL);
        }
        else
            execl("./scheduler.out", "./scheduler.out", selectedAlgoStr, numProcessesStr, NULL);
    }

    sleep_ms(100);

    int timer = getTime();
    Process *nextProcess = q_peek(pQueue);

    while (true)
    {
        if (timer == getTime()) {
            continue;
        }

        timer++;

        while (nextProcess != NULL && timer == nextProcess->arrivalTime)
        {
            sendMessageToScheduler(msgQueueID, nextProcess);
            q_dequeue(pQueue);
            nextProcess = q_peek(pQueue);
        }

        kill(schedulerId, SIGUSR1);

        sleep_ms(500);
    }

    return 0;
}

void clearResources(int signum)
{
    msgctl(msgQueueID, IPC_RMID, (struct msqid_ds *)0);
    kill(schedulerId, SIGINT);
    disconnectClk(true);
    exit(0);
}

void initalizeQueue(char *filePath, Queue *pQueue)
{
    Process *newProcess;
    FILE *fptr;

    fptr = safe_fopen(filePath, "r");

    int id, arrival, runtime, priority,memsize;
    char ch;

    while ((ch = fgetc(fptr)) != EOF)
    {
        if (ch == '#')
        {
            fscanf(fptr, "%*[^\n]");
        }
        else
        {
            if ((ch = fgetc(fptr)) != EOF)
            {
                ungetc(ch, fptr);
                fscanf(fptr, "%d %d %d %d %d", &id, &arrival, &runtime, &priority,&memsize);
                newProcess = (Process *)malloc(sizeof(Process));
                newProcess->arrivalTime = arrival;
                newProcess->id = id;
                newProcess->runningTime = runtime;
                newProcess->priority = priority;
                newProcess->memsize = memsize;
                q_enqueue(pQueue, newProcess);
                printf("Successfully read process with id %d\n", id);
            }
        }
    }
}

void sendMessageToScheduler(int msgQueueID, Process *newProcess)
{
    processMsg msg = {.mtype = SCHEDULER_TYPE, .newProcess = *newProcess};

    int send_val = msgsnd(msgQueueID, &msg, sizeof(Process), !IPC_NOWAIT);

    if (send_val == -1)
    {
        perror("Errror in sending message to scheduler!");
    }
}
