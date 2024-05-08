#include <stdbool.h>

struct PriorityQueue;

typedef struct PriorityQueue PriorityQueue;

void *pq_create(int size, bool comp(void *, void *));

void pq_destroy(void *heap);

void pq_enqueue(void *heap, void *k);

void *pq_top(void *heap);

void *pq_at(void *heap, int index);

void pq_dequeue(void *heap);

void pq_remove(void *heap, int k);

void pq_remove_element(void *heap, void *element);

int pq_size(void *heap);

bool pq_empty(void *heap);