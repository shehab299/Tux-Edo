#include "../src/DataStructures/priorityQ.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct dummy_{
    int time;
    int order;
};

typedef struct dummy_ dummy;

bool comp(void* x , void* y)
{
        dummy* d1 = (dummy*) x;
        dummy* d2 = (dummy*) y;
        return d1->time < d2->time;
}


int main(int argc, char const *argv[])
{

    Heap* myHeap = create_heap(150 , comp);
    dummy* array[3];

    array[0] = malloc(sizeof(dummy));
    array[1] = malloc(sizeof(dummy));
    array[2] = malloc(sizeof(dummy));

    array[0]->time = 0;
    array[1]->time = 1;
    array[2]->time = 2;
    
    insert(array[0],myHeap);
    insert(array[1],myHeap);
    insert(array[2],myHeap);

    deleteMin(myHeap);

    dummy* x = minElement(myHeap);
    printf("%d \n" , x->time);

    return 0;
}
