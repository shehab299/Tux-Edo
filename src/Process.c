#include "Includes/defs.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int timer;
int process_msgQueueID;

void killed(int signum)
{
    signal(SIGINT, killed);
    exit(0);
}

void cont(int signum)
{
    timeMsg receivedTime;
    msgrcv(process_msgQueueID, &receivedTime, sizeof(int), TIME_PROCESS_TYPE, !IPC_NOWAIT);
    timer = receivedTime.time;
    signal(SIGUSR1, cont);
}

int main(int argc, char const *argv[])
{
    signal(SIGINT, killed);
    signal(SIGUSR1, cont);
    process_msgQueueID = createMessageQueue(TIME_MESSAGE);
    connectToClk();

    int executionTime = atoi(argv[1]);
    timer = getTime();

    while (executionTime != 0)
    {
        if (timer < getTime())
        {
            timer++;
            executionTime--;
        }
    }

    kill(getppid(), SIGUSR2);

    disconnectClk(false);
    exit(0);
    return 0;
}
