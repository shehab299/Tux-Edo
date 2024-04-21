#include "Includes/UI.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "DataStructures/Queue.h"
#include "Includes/defs.h"

Queue* inputFile();
void clearResources(int);
int createMessageQueue();
void sendMessageToScheduler(int, processMsg*, Process*);

int msgQueueID;

/*

        To run this function you need to pass an argument which is the absolute path of the project
        like : /home/rabie/Desktop/Tux-Edo

*/

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);

    // Reading input file
    Queue* pQueue = inputFile();

    // Taking user input for choice of scheduling algorithm and parameters if needed
    int selectedAlgo = userInput();

    char *absolutePath = argv[1];

    // Forking clock process and changing core image
    int clkPid = fork();
    if (clkPid == -1)
    {
        perror("Error in forking");
        exit(-1);
    }
    else if (clkPid == 0)
    {
        execl(strcat(argv[1], "/clk.out"), "clk", NULL);
    }

    // Forking scheduler process and changing core image
    int schedulerPid = fork();
    if (schedulerPid == -1)
    {
        perror("Error in forking");
        exit(-1);
    }
    else if (schedulerPid == 0)
    {
        execl(strcat(argv[1], "/scheduler.out"), "scheduler", NULL);
    }

    connectToClk();

    // Creating message queue for communication with Scheduler
    msgQueueID = createMessageQueue();
    processMsg msg;
    msg.mtype = schedulerPid % 10000;

    // TODO Generation Main Loop
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources

    int time;
    Process* nextProcess;

    while((nextProcess = peek(pQueue)) != NULL) {

        time = getTime();
        printf("current time is %d\n", time);

        while (nextProcess && time == nextProcess->arrivalTime) {
            kill(schedulerPid, SIGUSR1);
            sendMessageToScheduler(msgQueueID, &msg, nextProcess);
            dequeue(pQueue);
            nextProcess = peek(pQueue);
        }
        
        sleep_ms(500);
    }
    
    printf("ProcessGen: done reading and sending.\n");

    int stat_loc;
    waitpid(schedulerPid, &stat_loc, 0);

    if (!(stat_loc & 0x00FF))
        printf("ProcessGen: Scheduler terminated safely with exit code\n", stat_loc >> 8);

    disconnectClk(true);
    return 0;
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    msgctl(msgQueueID, IPC_RMID, (struct msqid_ds *)0);
    disconnectClk(true);
    exit(0);
}

Queue* inputFile()
{
    Process* newProcess;
    FILE *fptr;
    fptr = fopen("processes.txt", "r");
    if (fptr == NULL)
        exit(-1);
    int id, arrival, runtime, priority;
    char ch;
    Queue* pQueue = createQueue();

    while ((ch = fgetc(fptr)) != EOF)
    {
        if (ch == '#')
        {
            fscanf(fptr, "%*[^\n]");
        }
        else
        {
            ungetc(ch, fptr);
            fscanf(fptr, "%d %d %d %d", &id, &arrival, &runtime, &priority);

            newProcess = (Process *)malloc(sizeof(Process));
            newProcess->arrivalTime = arrival;
            newProcess->id = id;
            newProcess->runningTime = runtime;
            newProcess->priority = priority;

            enqueue(pQueue, newProcess);

            printf("Successfully read process with id %d\n", id);
        }
    }

    return pQueue;
}

int createMessageQueue() {
    key_t key_id = ftok("../keyfile", 65);
    int msgQueueID = msgget(key_id, 0666 | IPC_CREAT);

    if (msgQueueID == -1)
    {
        perror("Error in creating message queue!");
        exit(-1);
    }

    printf("ProcessGen: message queue created with id %d\n", msgQueueID);

    return msgQueueID;
}

void sendMessageToScheduler(int msgQueueID, processMsg* msg, Process* newProcess) 
{
    msg->newProcess = *newProcess;
    int send_val = msgsnd(msgQueueID, msg, sizeof(msg->newProcess), !IPC_NOWAIT);

    if (send_val == -1)
        perror("Errror in sending message to scheduler!");

    printf("ProcessGen: message sent to scheduler with process id %d\n", newProcess->id);
}
