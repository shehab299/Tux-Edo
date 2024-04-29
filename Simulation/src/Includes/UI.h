#ifndef _inputDefinition
#define _inputDefinition

#include <stdlib.h>
#include <stdio.h>
#include <termio.h>
#include <unistd.h>

#define KEY_UP 'A'
#define KEY_DOWN 'B'
#define KEY_ENTER 10
#define true 1
#define false 0
#endif

int getch(void)
{
    int ch;
    struct termios oldt;
    struct termios newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void userInput(int *selectedAlgo, int *timeSlice)
{
    int numChoices = 3;
    char *choices[] = {"Non-preemptive Highest Priority First", "Shortest Remaining time Next", "Round Robin"};
    int selectedChoice = 0;
    int selected = false;
    while (!selected)
    {
        system("clear");
        printf("\033[33mSelect an algorithm from the following\033[0m\n");
        for (int i = 0; i < numChoices; i++)
        {
            printf("%s", (i == selectedChoice) ? "-> " : "   ");
            if (i == selectedChoice)
            {
                printf("\033[31m%s\033[0m\n", choices[i]);
            }
            else
            {
                printf("%s\n", choices[i]);
            }
        }
        int key = getch();
        switch (key)
        {
        case 27:
            getch();
            switch (getch())
            {
            case 'A':
                selectedChoice = (selectedChoice - 1 + numChoices) % numChoices;
                break;
            case 'B':
                selectedChoice = (selectedChoice + 1) % numChoices;
                break;
            }
            break;
        case KEY_ENTER:
            selected = true;
            break;
        }
    }
    *selectedAlgo = selectedChoice + 1;
    if (*selectedAlgo == 3)
    {
        printf("\n");
        char ch;
        do
        {
            printf("\033[35mEnter the time slice for RR algorithm\033[00m\n");
            if (!scanf("%d", timeSlice))
            {
                printf("\033[34mInvalid input. Please enter an integer\033[00m\n\033[35mAgain ");
                while (getchar() != '\n')
                    ;
            }
            else if (*timeSlice <= 0)
                printf("\033[31mInvalid input. Please enter an integer greater than 0\033[00m\n\033[35mAgain ");
        } while (*timeSlice <= 0);
    }
}