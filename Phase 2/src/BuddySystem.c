#include "Includes/defs.h"
#include "BuddySystem.h"
#include "DataStructures/PriorityQueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
typedef struct BuddySystem
{
    void *slots;
    int totalSize;
    int freeSize;
    int smallestFreeSize;
    int largestFreeSize;

} BuddySystem;

bool compare(void *_first, void *_second)
{
    struct Slot *first = (struct Slot *)_first;
    struct Slot *second = (struct Slot *)_second;
    if (first->pcb != NULL && second->pcb != NULL)
        return first->start < second->start;
    else if (first->pcb != NULL)
        return false;
    else if (second->pcb != NULL)
        return true;
    else if (first->size == second->size)
        return first->start < second->start;
    else
        return first->size < second->size;
}

void *createSlot(int start, int end, void *pcb)
{
    Slot *slot = (Slot *)malloc(sizeof(Slot));
    slot->start = start;
    slot->end = end;
    slot->size = end - start;
    slot->pcb = (PCB *)pcb;
    return (void *)slot;
}

void *createBuddySystem(int memorySize)
{
    BuddySystem *sys = (BuddySystem *)malloc(sizeof(BuddySystem));
    sys->totalSize = memorySize;
    sys->smallestFreeSize = memorySize;
    sys->freeSize = memorySize;
    sys->largestFreeSize = memorySize;
    PriorityQueue *queue = pq_create(1, compare);
    sys->slots = queue;
    void *slot = createSlot(0, memorySize, NULL);
    pq_enqueue(&queue, slot);
    return sys;
}

void splitSlot(void *_queue, void *_leftSlot)
{

    PriorityQueue **queue = (PriorityQueue **)_queue;

    Slot *leftSlot = (Slot *)_leftSlot;
    Slot *rightSlot = malloc(sizeof(Slot));
    int slotSize = leftSlot->size;
    int start = leftSlot->start;
    int end = leftSlot->end;

    rightSlot->start = start + slotSize / 2;
    rightSlot->end = end;
    rightSlot->size = slotSize / 2;
    rightSlot->pcb = NULL;

    pq_enqueue(queue, rightSlot);

    leftSlot->start = start;
    leftSlot->end = start + slotSize / 2;
    leftSlot->size = slotSize / 2;
}

bool allocateProcess(void *_buddySystem,void* _pcb)
{

    BuddySystem *buddySystem = (BuddySystem *)_buddySystem;
    PriorityQueue *queue = (PriorityQueue *)buddySystem->slots;
    PCB *pcb = (PCB *)_pcb;

    int requiredSize = pow(2, ceil(log(pcb->allocatedSize) / log(2)));

    int index = 0;
    while (index < pq_size(queue))
    {
        Slot *slot = ((Slot *)pq_at(queue, index));
        if (slot->size >= requiredSize)
        {
            pq_remove(queue, index);
            while (slot->size > requiredSize)
            {
                splitSlot(&queue, slot);
                buddySystem->smallestFreeSize = (slot->size < buddySystem->smallestFreeSize) ? slot->size : buddySystem->smallestFreeSize;
            }
            slot->pcb = pcb;
            pq_enqueue(&queue, slot);
            break;
        }
        index++;
    }
    buddySystem->slots = queue;
    _buddySystem = buddySystem;
}

void deallocateProcess(void *_buddySystem,void * _pcb)
{
    BuddySystem *buddySystem = (BuddySystem *)_buddySystem;
    PCB *pcb = (PCB *)_pcb;
    PriorityQueue *queue = (PriorityQueue *)buddySystem->slots;
    int index = 0;

    while (index < pq_size(queue))
    {
        Slot *slot = (Slot *)pq_at(queue, index);
        if(slot->pcb != NULL &&slot->pcb->id == pcb->id)
        {
            pq_remove(queue, index);
            slot->pcb = NULL;
            pq_enqueue(&queue, slot);
            break;
        }
        index++;
    }
    buddySystem->slots = queue;
    _buddySystem = buddySystem;
}

void print(void *_buddySystem)
{
    BuddySystem *buddySystem = (BuddySystem *)_buddySystem;
    PriorityQueue *queue = buddySystem->slots;
    for (int i = 0; i < pq_size(queue); i++)
    {
        Slot *slot = (Slot *)pq_at(queue, i);
        printf("%d Start = %d End = %d Size = %d\n", i, slot->start, slot->end, slot->size);
    }
    printf("---------------------------------------------------------------------\n");
}
