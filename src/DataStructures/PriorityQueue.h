#include <stdbool.h>

struct PriorityQueue;

typedef struct PriorityQueue PriorityQueue;

void* pq_create(int size,bool comp(void*,void*));

void pq_destroy(void* heap);

void pq_enqueue(void* k, void* heap);

void* pq_top(void* heap);

void pq_dequeue(void* heap);

int pq_size(void* heap);

bool pq_empty(void* heap);