//List.h

typedef struct node {
	struct node* next;
	unsigned int index;
} node;

typedef struct LRU {
	struct node* head;
} LRU;

