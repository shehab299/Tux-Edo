#include <stdio.h>
#include <stdlib.h>

void inputFile()
{
    FILE *fptr;
    fptr = fopen("processes.txt", "r");
    if (fptr == NULL)
        exit(-1);
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
        }
    }
}