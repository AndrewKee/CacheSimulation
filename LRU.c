//LRU Linked List

// #include "LRU.h"
#include "cache.h"

LRU* LRU_Const(unsigned int num_block)
{
	if (num_block)
	{
		struct LRU* lru = (struct LRU)malloc(sizeof(struct LRU));

		struct node* n_ptr;

		for (unsigned int i = 0; i < num_block; i++)
		{
			n_ptr[i] = (struct node)malloc(sizeof(struct node));
			n_ptr[i].index = i;
			if(i)
				n_ptr[i-1].next = n_ptr[i];
		}
		lru->head = n_ptr[0];
		lru->tail = n_ptr[num_block-1];
		return lru;
	}

	else
	{
		return NULL;
	}
}

void LRU_Update(struct cache* cache_level, unsigned int set, unsigned int index){
	if (!index)
	{
		return;
	}

	struct node* cur_ptr;
	struct node* i_ptr;

	cur_ptr = cache_level->cache_set[set].lru->head;

	for (unsigned int i = 0; i < index - 1; i++)
	{
		cur_ptr = cur_ptr->next;
		if (!cur_ptr && !cur_ptr->next) return; //Bad error checking
	}

	i_ptr = cur_ptr->next;

	if (cur_ptr->next) 
	{
		cur_ptr = cur_ptr->next->next;
	}

	i_ptr->next = cache_level->cache_set[set].lru->head;
	cache_level->cache_set[set].lru->head = i_ptr;

	while(cur_ptr->next) cur_ptr = cur_ptr->next;

	cache_level->cache_set[set].lru->tail = cur_ptr;
}

LRU* LRU_getLRU(struct LRU *lru){

}