typedef struct node {
	struct node* next;
	unsigned int index;
} node;

typedef struct LRU {
	struct node* head;
	struct node* tail;
} LRU;

LRU* LRU_Const(unsigned int num_block);

void LRU_Update(struct cache* cache_level, unsigned int set, unsigned int index);

LRU* LRU_getLRU(struct LRU *lru);