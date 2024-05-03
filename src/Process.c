#include "Includes/defs.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>


int timer;

void killed(int signum)
{
    signal(SIGINT, killed);
    exit(0);
}


int main(int argc, char const *argv[])
{
    signal(SIGINT, killed);

    connectToClk();

    int executionTime = atoi(argv[1]);
    timer = getTime();

    while (executionTime > 0)
    {
        if (timer != getTime())
        {
            timer++;

            if(timer != getTime()){
                timer = getTime();
            }

            executionTime--;
        }
    }

    kill(getppid(), SIGUSR2);

    disconnectClk(false);
    exit(0);
    return 0;
}
