#include "Includes/defs.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int main(int argc, char const *argv[])
{
    
    connectToClk();

    int executionTime = atoi(argv[1]);
    int timer = getTime();
    

    printf("process started \n");
    while (executionTime != 0)
    {
        if(timer != getTime())
        {
            timer++; 
            //Q what if I am interrupted?
            executionTime--;
        }
    }
    printf("process finished \n");

    //ON TERMINATION SENDS SIGNAL (SIGUSR1) TO PARENT
    kill(getppid(),SIGUSR1);


    disconnectClk(false);    
    return 0;
}
