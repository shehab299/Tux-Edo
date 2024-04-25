#include "../src/DataStructures/CircularQueue.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    CircularQueue* queue = cq_create();

    int* newInt = malloc(sizeof(int));
    *newInt = 3;

    int* newInt2 = malloc(sizeof(int));
    *newInt2 = 2;

    int* newInt3 = malloc(sizeof(int));
    *newInt3 = 1;

    cq_enqueue(queue,newInt);
    cq_enqueue(queue,newInt2);
    cq_enqueue(queue,newInt3);

    cq_printQueue(queue);

    cq_dequeue(queue);

    cq_printQueue(queue);

    cq_dequeue(queue);

    cq_printQueue(queue);

    cq_dequeue(queue);

    cq_printQueue(queue);

    printf("%d" , cq_size(queue));
    printf("%d" , cq_empty(queue));

    return 0;
}
