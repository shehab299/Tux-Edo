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
    slot->size = end - start + 1;
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
    sys->slots = pq_create(20, compareSlot);
    void *slot = createSlot(0, memorySize - 1, NULL, NULL);
    pq_enqueue(sys->slots, slot);
    return sys;
}

void *splitSlot(void *_queue, void *_parentSlot)
{

    PriorityQueue *queue = (PriorityQueue *)_queue;

    Slot *parentSlot = (Slot *)_parentSlot;
    int slotSize = parentSlot->size / 2;
    int start = parentSlot->start;
    int end = parentSlot->end;

    Slot *leftSlot = createSlot(start, start + slotSize - 1, NULL, _parentSlot);
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
    {
        pq_enqueue(_queue, childSlot1);
        return;
    }

    PriorityQueue *queue = (PriorityQueue *)_queue;
    Slot *parentSlot = (Slot *)childSlot1->parent;
    while (parentSlot != NULL)
    {
        Slot *childSlot2 = (childSlot1 == (Slot *)parentSlot->leftHalf) ? (Slot *)parentSlot->rightHalf : (Slot *)parentSlot->leftHalf;
        if (childSlot2->leftHalf == NULL && childSlot2->rightHalf == NULL && childSlot2->pcb == NULL && childSlot1->leftHalf == NULL && childSlot1->rightHalf == NULL && childSlot1->pcb == NULL)
        {
            pq_remove_element(queue, childSlot2);
            pq_remove_element(queue, childSlot1);
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
}

bool allocateProcess(void *_buddySystem, void *_pcb, int memsize, int *startLocation, int *endLocation)
{

    BuddySystem *buddySystem = (BuddySystem *)_buddySystem;
    PriorityQueue *queue = (PriorityQueue *)buddySystem->slots;

    int requiredSize = pow(2, ceil(log(memsize) / log(2)));
    bool success = false;

    if (requiredSize <= buddySystem->freeSize)
    {
        int index = 0;
        while (index < pq_size(queue))
        {
            Slot *slot = ((Slot *)pq_at(queue, index));
            if (slot->pcb != NULL)
                break;
            if (slot->size >= requiredSize)
            {
                pq_remove(queue, index);
                while (slot->size > requiredSize)
                {
                    slot = splitSlot(buddySystem->slots, slot);
                }
                buddySystem->slots = queue;
                slot->pcb = _pcb;
                pq_enqueue(buddySystem->slots, slot);
                *startLocation = slot->start;
                *endLocation = slot->end;
                success = true;
                buddySystem->freeSize -= requiredSize;
                break;
            }
            index++;
        }
    }
    return success;
}

void deallocateProcess(void *_buddySystem, void *_pcb, int memsize)
{
    BuddySystem *buddySystem = (BuddySystem *)_buddySystem;
    PriorityQueue *queue = (PriorityQueue *)buddySystem->slots;
    int index = 0;
    int releasedSize = pow(2, ceil(log(memsize) / log(2)));

    while (index < pq_size(queue))
    {
        Slot *slot = (Slot *)pq_at(queue, index);
        if (slot->pcb != NULL && slot->pcb == _pcb)
        {
            buddySystem->freeSize += releasedSize;
            pq_remove(queue, index);
            slot->pcb = NULL;

            mergeSlot(queue, slot);
            break;
        }
        index++;
    }
}