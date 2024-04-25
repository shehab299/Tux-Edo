#include "./Includes/defs.h"
#include "DataStructures/priorityQ.h"

#define READY 1
#define RUNNING 2
#define TERMINATED 3

//FROM WHERE THE PROCESSES COME?
//Maybe each second I'll check the msgqueue

void processTerminates(int sigNum)
{
    //DONNO YET
    signal(SIGUSR1, processTerminates);
}

typedef struct Scheduler
{
    PCB *processes;
    int currentTime;
    int numProcesses;
    int cpuUtilization;
    float avgWTA;
    float avgTA;
    int standardDeviation;
    float totalTA;
    float totalWTA;

} Scheduler;

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

void HPF(Scheduler *scheduler)
{
    Heap *pq = create_heap(scheduler->numProcesses, true);

    for (int i = 0; i < scheduler->numProcesses; i++)
    {
        if (scheduler->processes[i].state == READY)
        {
            insert((void *)&scheduler->processes[i], pq);
        }
    }

    while (1)
    {
        //check if a new process arrived
        //add it to the queue
        //Okay, when to stop hehe
        //wait, I'll run forever!?
        //what if no processes?
        if (scheduler->currentTime != getTime())
        {
            //check if a process arrived
            //add it to the queue
            scheduler->numProcesses++;
            scheduler->currentTime = getTime();
        }

        if (!isEmpty(pq))
        {
            // replace it with function runProcess();
            PCB *running = (PCB *)minElement((void *)pq);
            running->state = RUNNING;
            running->remainingTime = running->runningTime;
            int pid = fork();

            if (pid == -1)
            {
                perror("Error in forking process");
                exit(-1);
            }
            else if (pid == 0)
            {
                system("gcc ./Process.c -o process");
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

                deleteMin(pq);
                // delete its PCB
            }
        }
    }
}

Scheduler *createScheduler(int numProcesses)
{
    Scheduler *scheduler = (Scheduler *)malloc(sizeof(Scheduler));
    scheduler->processes = (PCB *)malloc(numProcesses * sizeof(PCB));
    scheduler->numProcesses = numProcesses;
    scheduler->cpuUtilization = 0.0;
    scheduler->currentTime = getTime();
    scheduler->totalTA = 0.0;
    scheduler->totalWTA = 0.0;
    scheduler->avgWTA = 0.0;
    scheduler->avgTA = 0.0;
    scheduler->standardDeviation = 0.0;

    scheduler->processes[0] = (PCB){NULL,1, 0, 5, 4, 5, 0, 0, 0.0, 0, 0, READY};
    scheduler->processes[1] = (PCB){NULL,2, 1, 3, 4, 3, 0, 0, 0.0, 0, 0, READY};
    scheduler->processes[2] = (PCB){NULL,3, 2, 6, 4, 6, 0, 0, 0.0, 0, 0, READY};
    scheduler->processes[3] = (PCB){NULL,4, 3, 2, 4, 2, 0, 0, 0.0, 0, 0, READY};
    scheduler->processes[4] = (PCB){NULL,5, 4, 4, 4, 4, 0, 0, 0.0, 0, 0, READY};
}

int main(int argc, char *argv[])
{
    signal(SIGUSR1, processTerminates);
    // key_t key = ftok("./keyFile", 1);
    // int msgqid = msgget(key, IPC_CREAT | 0666);
    // if (msgqid == -1)
    // {
    //     perror("Error in creating msg queue");
    //     exit(-1);
    // }

    connectToClk();
    Scheduler *scheduler = createScheduler(5);
    disconnectClk(true);
// =======
// #include "Includes/defs.h"

// int rec_val, msgQueueID;
// processMsg msg;

// void processMessageReceiver(int signum) {
//     while((rec_val = msgrcv(msgQueueID, &msg, sizeof(msg.newProcess), getpid() % 10000, IPC_NOWAIT)) != -1 || errno != ENOMSG) 
//     {
//         if (errno == ENOMSG) {
//             printf("Scheduler: Message queue is empty\n");
//             break;
//         }
        
//         printf("Scheduler: Message received! with process id %d\n", msg.newProcess.id);
//     }
// /*     if (rec_val == -1)
//         perror("Error in receiving message from ProcessGen!"); */

//     signal(SIGUSR1, processMessageReceiver);
// }

// void clearResources(int signum) {
//     exit (0);
//     signal(SIGINT, clearResources);
// }

// int main() {
//     signal(SIGUSR1, processMessageReceiver);
//     signal(SIGINT, clearResources);
//     printf("Hello from scheduler!\n");

//     connectToClk();

//     key_t key_id = ftok("../keyfile", 65);
//     msgQueueID = msgget(key_id, 0666 | IPC_CREAT);

//     if (msgQueueID == -1)
//     {
//         perror("Error in creating message queue!");
//         exit(-1);
//     }

//     printf("Scheduler: message queue created with id %d\n", msgQueueID);

//     msg.mtype = getpid() % 10000;

//     while(getTime() < 35) 
//     {
//         sleep_ms(950);
//         printf("Scheduler still running\n");
//     }

//     disconnectClk(true);

//     
}