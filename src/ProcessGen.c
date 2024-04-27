#include "Includes/UI.h"
#include "DataStructures/Queue.h"
#include "Includes/defs.h"

void clearResources(int);
int createMessageQueue();
void sendMessageToScheduler(int, Process *);
void initalizeQueue(char* filePath, Queue* queue);



int msgQueueID;

int schedulerId;

int main(int argc, char *argv[])
{
    msgQueueID = createMessageQueue();

    signal(SIGINT, clearResources);

    if(argc < 2)
    {
        perror("Didn't Provide The Input File");
        exit(-1);
    }

    Queue* pQueue = createQueue(); 
    initalizeQueue("./processes.txt",pQueue);


    int selectedAlgo = userInput();
    printf("%d \n",q_getSize(pQueue));

    
    int clkPid = safe_fork();

    if (clkPid == 0)
        execl("./clk.out", "./clk.out" , NULL);

    connectToClk();

    schedulerId = fork();

    if (schedulerId == 0)
    {
        char selectedAlgoStr[10];
        sprintf(selectedAlgoStr, "%d", selectedAlgo);
        execl("/home/shehab/Tux-Edo/scheduler.out", "./scheduler.out", selectedAlgoStr, NULL);
    }

    sleep_ms(100);

    //FINISH WORK
    int timer = getTime();
    Process* nextProcess = q_peek(pQueue);

    while(true)
    {
        if(timer == getTime())
            continue;
        
        int send = 0;
        timer++;
        // printf("Time Is %d \n" , timer);

        while (nextProcess != NULL && timer == nextProcess->arrivalTime)
        {
            // printf("1 \n");
            sendMessageToScheduler(msgQueueID,nextProcess);
            q_dequeue(pQueue);
            nextProcess = q_peek(pQueue);

            send = 1;
        }

        printf("sending signal \n");
        kill(schedulerId, SIGUSR1);

        sleep_ms(500);
    }

    return 0;
}

void clearResources(int signum)
{
    msgctl(msgQueueID, IPC_RMID, (struct msqid_ds *)0);
    kill(schedulerId,SIGINT);
    disconnectClk(true);
    exit(0);
}

void initalizeQueue(char* filePath, Queue* pQueue)
{
    Process *newProcess;
    FILE *fptr;

    fptr = safe_fopen(filePath, "r");
    
    int id, arrival, runtime, priority;
    char ch;

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

            q_enqueue(pQueue, newProcess);

            printf("Successfully read process with id %d\n", id);
        }
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

    printf("ProcessGen: message queue created with id %d\n", msgQueueID);

    return msgQueueID;
}

void sendMessageToScheduler(int msgQueueID, Process *newProcess)
{
    processMsg msg = {.mtype = SCHEDULER_TYPE , .newProcess = *newProcess};

    int send_val = msgsnd(msgQueueID, &msg, sizeof(Process), !IPC_NOWAIT);

    if (send_val == -1)
        perror("Errror in sending message to scheduler!");

    // printf("ProcessGen: message sent to scheduler with process id %d\n", newProcess->id);
}
