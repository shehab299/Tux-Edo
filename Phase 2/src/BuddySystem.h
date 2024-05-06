#include<stdbool.h>

struct BuddySystem;

typedef struct BuddySystem BuddySystem;


void *createBuddySystem(int memorySize);
void splitSlot(void *_queue, void *_leftSlot);
bool allocateProcess(void *_buddySystem, void* _pcb);
void deallocateProcess(void *_buddySystem, void* _pcb);

void print(void *_buddySystem);
