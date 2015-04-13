//LRU Linked List
#include "LRU.h"
// #include "cache.h"
#include <stdio.h>
#include <stdlib.h>

LRU* LRU_Construct(unsigned int num_block)
{
	
	if (num_block)
	{
		struct LRU* lru = malloc( sizeof(struct LRU));
		struct node* n_ptr = NULL;
		n_ptr = malloc(num_block * sizeof(struct node));

		unsigned int i;
		for (i = 0; i < num_block; i++)
		{
			n_ptr->index = num_block - 1 - i;
			n_ptr->next = NULL;
			if(i) {
				struct node* l_ptr = n_ptr;
				l_ptr--;
				l_ptr->next = n_ptr;
			}

			if (!i) lru->head = n_ptr;

			n_ptr++;
		}

		return lru;
	}
	return NULL;
}

node* LRU_Update(cache* cache_level, uint set, uint block){
	//printf("%u %u\n", set, block);
	if (block > cache_level->assoc) 
	{
		printf("Block index exceeds associativity: ERROR");
		return NULL;
	}

	struct node* cur_ptr;
	struct node* i_ptr;

	//Set the pointer equal to the head
	cur_ptr = cache_level->cache_set[set].lru->head;

	//Check for single element list, or if the way is already most recently used

	if (!cur_ptr->next || cur_ptr->index == block) return cur_ptr;
	
	while(cur_ptr->next)
	{
		//Stop when cur_ptr is equal to the prior element than the block
		if (cur_ptr->next->index == block)
		{
			break;
		}
		else
		{
			//Move on to the next element
			cur_ptr = cur_ptr->next;
		}
	}

	//i_ptr becomes a pointer to the block that needs to be moved to the top
	i_ptr = cur_ptr->next;
	// printf("cur index: %u \n", cur_ptr->index);
	// printf("i index: %u \n", i_ptr->index);
	//If we are not at the final element, we should link cur_ptr to the element after i_ptr
	if (cur_ptr->next->next) 
	{
		//The next pointer should skip i_ptr
		cur_ptr->next = cur_ptr->next->next;
	}
	else{
		cur_ptr->next = NULL;
	}

	//Point i_ptr to the current head
	i_ptr->next = cache_level->cache_set[set].lru->head;

	//Link the head to i_ptr
	cache_level->cache_set[set].lru->head = i_ptr;

	return i_ptr;
}

unsigned int LRU_Get_LRU(cache* cache_level, uint set)
{	
	// printf("1\n");
	struct node* cur_ptr = cache_level->cache_set[set].lru->head;
	// printf("index: %u\n", cur_ptr->index);
	//Stuck in an infinite loop here, why?
	while (cur_ptr != NULL && cur_ptr->next != NULL)
	{	
		// printf("loop %d\n", cur_ptr->next->index);
		cur_ptr = cur_ptr->next;
	}
	// printf("3\n");
	return cur_ptr->index;
}
