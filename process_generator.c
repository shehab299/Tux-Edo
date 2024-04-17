#include "headers.h"
#include "userInput.h"
#include "inputFile.h"
#include <unistd.h>
#include <string.h>

#ifndef _SchedulingAlgorithm
#define _SchedulingAlgorithm
#define HPF 1
#define STRN 2
#define RR 3
#endif

void clearResources(int);
/*

        To run this function you need to pass an argument which is the absolute path of the project
        like : /home/rabie/Desktop/Tux-Edo

*/
int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    inputFile();
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int selectedAlgo = userInput();
    // 3. Initiate and create the scheduler and clock processes.
    char *absolutePath = argv[1];
    system("gcc clk.c -o clk");
    system("gcc scheduler.c -o scheduler");
    int pid = fork();
    if (pid == -1)
    {
        perror("Error in forking");
        exit(-1);
    }
    else if (pid == 0)
    {
        execl(strcat(argv[1], "/clk"), "clk", NULL);
    }

    pid = fork();
    if (pid == -1)
    {
        perror("Error in forking");
        exit(-1);
    }
    else if (pid == 0)
    {
        execl(strcat(argv[1], "/scheduler"), "scheduler", NULL);
    }

    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
}
