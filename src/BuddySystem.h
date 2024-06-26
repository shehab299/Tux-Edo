#include <stdbool.h>
typedef struct Slot
{
    int start;
    int end;
    int size;
    void *pcb;
    void *leftHalf;
    void *rightHalf;
    void *parent;

} Slot;

struct BuddySystem;

typedef struct BuddySystem BuddySystem;

void *createBuddySystem(int memorySize);

void *splitSlot(void *_queue, void *_leftSlot);

bool allocateProcess(void *_buddySystem, void *_pcb, int memsize, int *startLocation, int *endLocation);

void deallocateProcess(void *_buddySystem, void *_pcb, int memsize);

void print(void *_buddySystem);
