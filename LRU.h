struct cache;
typedef struct cache cache; 
typedef unsigned int uint;

typedef struct node {
	struct node* next;
	unsigned int index;
} node;

typedef struct LRU {
	struct node* head;
} LRU;

// Initializes an LRU structure to hold the least recently used block
LRU* LRU_Construct(unsigned int num_block);

//reorganizes the LRU to put the least recently used block at the top and return that node
node* LRU_Update(cache* cache_level, uint set, uint block);

unsigned int LRU_Get_LRU(cache* cache_level, uint set);