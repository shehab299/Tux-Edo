#include "BuddySystem.h"
#include"DataStructures/PriorityQueue.h"
#include"DataStructures/Queue.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// waiting queue is out


typedef struct BuddySystem
{
    void *slots;
    void *waitingQueue;
    int totalSize;
    int freeSize;
    int smallestFreeSize;
    int largestFreeSize;

} BuddySystem;

bool compareSlot(void *_first, void *_second)
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

void *createSlot(int start, int end, void *pcb, void *_parent)
{
    Slot *slot = (Slot *)malloc(sizeof(Slot));
    slot->start = start;
    slot->end = end;
    slot->size = end - start;
    slot->pcb = pcb;
    slot->leftHalf = slot->rightHalf = NULL;
    slot->parent = _parent;
    return slot;
}

void *createBuddySystem(int memorySize)
{
    BuddySystem *sys = (BuddySystem *)malloc(sizeof(BuddySystem));
    sys->totalSize = memorySize;
    sys->smallestFreeSize = memorySize;
    sys->freeSize = memorySize;
    sys->largestFreeSize = memorySize;
    sys->waitingQueue = createQueue();
    sys->slots = pq_create(1, compareSlot);
    void *slot = createSlot(0, memorySize, NULL, NULL);
    pq_enqueue(sys->waitingQueue, slot);
    return sys;
}

void *splitSlot(void *_queue, void *_parentSlot)
{

    PriorityQueue **queue = (PriorityQueue **)_queue;

    Slot *parentSlot = (Slot *)_parentSlot;
    int slotSize = parentSlot->size / 2;
    int start = parentSlot->start;
    int end = parentSlot->end;

    Slot *leftSlot = createSlot(start, start + slotSize, NULL, _parentSlot);
    Slot *rightSlot = createSlot(start + slotSize, end, NULL, _parentSlot);

    parentSlot->leftHalf = leftSlot;
    parentSlot->rightHalf = rightSlot;

    pq_enqueue(queue, rightSlot);
    return leftSlot;
}

void mergeSlot(void *_queue, void *_slot)
{
    Slot *childSlot1 = (Slot *)_slot;
    if (childSlot1->parent == NULL)
        return;

    PriorityQueue **queue = (PriorityQueue **)_queue;
    Slot *parentSlot = (Slot *)childSlot1->parent;
    while (parentSlot != NULL)
    {
        Slot *childSlot2 = (childSlot1 == (Slot *)parentSlot->leftHalf) ? (Slot *)parentSlot->rightHalf : (Slot *)parentSlot->leftHalf;
        if (childSlot2->leftHalf == NULL && childSlot2->rightHalf == NULL && childSlot2->pcb == NULL && childSlot1->leftHalf == NULL && childSlot1->rightHalf == NULL && childSlot1->pcb == NULL)
        {
            pq_remove_element(*queue, childSlot2);
            pq_remove_element(*queue, childSlot1);
            free(childSlot1);
            free(childSlot2);
            parentSlot->leftHalf = parentSlot->rightHalf = NULL;
            childSlot1 = parentSlot;
            parentSlot = childSlot1->parent;
        }
        else
            break;
    }
    pq_enqueue(queue, childSlot1);
    _queue = queue;
}

bool allocateProcess(void *_buddySystem, void *_pcb,int memsize)
{

    BuddySystem *buddySystem = (BuddySystem *)_buddySystem;
    PriorityQueue *queue = (PriorityQueue *)buddySystem->slots;

    int requiredSize = pow(2, ceil(log(memsize) / log(2)));
    bool success = false;
    // if (requiredSize <= buddySystem->freeSize)
    //{
    int index = 0;
    while (index < pq_size(queue))
    {
        Slot *slot = ((Slot *)pq_at(queue, index));
        if (slot->size >= requiredSize)
        {
            // buddySystem->largestFreeSize = (slot->size / 2 > buddySystem->largestFreeSize) ? slot->size / 2 : buddySystem->largestFreeSize;

            pq_remove(queue, index);
            while (slot->size > requiredSize)
            {
                slot = splitSlot(&queue, slot);
                // buddySystem->smallestFreeSize = (slot->size < buddySystem->smallestFreeSize) ? slot->size : buddySystem->smallestFreeSize;
            }
            slot->pcb = _pcb;
            pq_enqueue(&queue, slot);
            success = true;
            //buddySystem->freeSize -= pcb->memsize;
            break;
        }
        index++;
    }
    buddySystem->slots = queue;
    return success;
    // }
    // else
    // return false;
}

void deallocateProcess(void *_buddySystem, void *_pcb)
{
    BuddySystem *buddySystem = (BuddySystem *)_buddySystem;
    PriorityQueue *queue = (PriorityQueue *)buddySystem->slots;
    int index = 0;

    while (index < pq_size(queue))
    {
        Slot *slot = (Slot *)pq_at(queue, index);
        if (slot->pcb != NULL && slot->pcb == _pcb)
        {
            pq_remove(queue, index);
            slot->pcb = NULL;
            // pq_enqueue(&queue, slot);
            mergeSlot(&queue, slot);
            break;
        }
        index++;
    }
}

void addProcessToWaiting(void *_buddySystem, void *_pcb)
{
    BuddySystem *buddySystem = (BuddySystem *)_buddySystem;
    q_enqueue(buddySystem->waitingQueue, _pcb);
}

void *peekWaitingQueue(void *_buddySystem)
{
    BuddySystem *buddySystem = (BuddySystem *)_buddySystem;
    return q_peek(buddySystem->waitingQueue);
}

void dequeWaitingQueue(void *_buddySystem)
{
    BuddySystem *buddySystem = (BuddySystem *)_buddySystem;
    q_dequeue(buddySystem->waitingQueue);
}

// void print(void *_buddySystem)
// {
//     BuddySystem *buddySystem = (BuddySystem *)_buddySystem;
//     PriorityQueue *queue = buddySystem->slots;
//     for (int i = 0; i < pq_size(queue); i++)
//     {
//         Slot *slot = (Slot *)pq_at(queue, i);
//         Slot *parent = (Slot *)slot->parent;
//         if (parent != NULL)
//             printf("%d Start = %d End = %d Size = %d  Allocated = %d  ----------- parent : Start=%d  End = %d \n", i, slot->start, slot->end, slot->size, (slot->pcb != NULL) ? slot->pcb->memsize : 0, parent->start, parent->end);
//         else
//             printf("%d Start = %d End = %d Size = %d  Allocated = %d\n", i, slot->start, slot->end, slot->size, (slot->pcb != NULL) ? slot->pcb->memsize : 0);
//     }
//     printf("Free Size = %d , smallest free size = %d , largest free siZe = %d\n", buddySystem->freeSize, buddySystem->smallestFreeSize, buddySystem->largestFreeSize);

//     printf("---------------------------------------------------------------------\n");
// }
