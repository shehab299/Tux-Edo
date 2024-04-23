#include "Includes/defs.h"

int rec_val, msgQueueID;
processMsg msg;

void processMessageReceiver(int signum) {
    while((rec_val = msgrcv(msgQueueID, &msg, sizeof(msg.newProcess), getpid() % 10000, IPC_NOWAIT)) != -1 || errno != ENOMSG) 
    {
        if (errno == ENOMSG) {
            printf("Scheduler: Message queue is empty\n");
            break;
        }
        
        printf("Scheduler: Message received! with process id %d\n", msg.newProcess.id);
    }
/*     if (rec_val == -1)
        perror("Error in receiving message from ProcessGen!"); */

    signal(SIGUSR1, processMessageReceiver);
}

void clearResources(int signum) {
    exit (0);
    signal(SIGINT, clearResources);
}

int main() {
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

    while(getTime() < 35) 
    {
        sleep_ms(950);
        printf("Scheduler still running\n");
    }

    disconnectClk(true);

    return 0;
}