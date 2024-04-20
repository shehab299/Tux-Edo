#include <stdbool.h>

typedef struct heap Heap;


Heap* create_heap(int size,bool comp(void*,void*));

void destory_heap(Heap* heap);

void insert(void* k, Heap* heap);

void* minElement(Heap* heap);

void deleteMin(Heap* heap);

int getCount(Heap* heap);

bool isEmpty(Heap* heap);