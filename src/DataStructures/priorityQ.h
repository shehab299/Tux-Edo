#include <stdbool.h>

typedef struct heap MinHeap;


MinHeap* create_heap(int size,bool comp(void*,void*));

void destory_heap(MinHeap* heap);

void insert(void* k, MinHeap* heap);

void* minElement(MinHeap* heap);

void deleteMin(MinHeap* heap);

int getCount(MinHeap* heap);

bool isEmpty(MinHeap* heap);