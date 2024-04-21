
typedef struct Queue Queue;
typedef struct Node Node;

void dequeue(Queue* q);
void enqueue(Queue* q, void* element);
void* peek(Queue*);
Queue* createQueue();



