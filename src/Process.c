#include "Includes/defs.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void killed(int signum)
{
    // printf("I'm killllleddddddddd\n");
    signal(SIGINT, killed);
    exit(0);
}

int main(int argc, char const *argv[])
{
    signal(SIGINT, killed);

    connectToClk();

    int executionTime = atoi(argv[1]);
    int timer = getTime();

    // printf("process started \n");
    while (executionTime != 0)
    {
        if (timer != getTime())
        {
            timer++;
            // printf("Process: Process still running at time: %d\n", getTime());
            executionTime--;
        }
    }
    // printf("process finished \n");

    // ON TERMINATION SENDS SIGNAL (SIGUSR1) TO PARENT
    kill(getppid(), SIGUSR2);

    disconnectClk(false);
    exit(0);
    return 0;
}
