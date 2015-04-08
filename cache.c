#include "cache.h"
// #include "LRU.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// #define DEBUG

int parse_config(char* filename, cache* l1_data, cache* l1_inst, cache* l2, cache* main_mem){
	FILE *fp;
	char input[20];
	char cacheLevel[10];
	int val;
	fp = fopen(filename, "r");

	if(!fp){
		printf("File Don't Exist \n");
		fp = fopen("Config/default.dat", "r");

		if (!fp) return -1;
	}
	else{  
		//read through the config file, 
		//the config file is formatted so that we have 
			// cacheLevel(string) property(string) value(integer)
		// repeated over new lines
		while (fscanf(fp,"%s %s %d\n",cacheLevel, input, &val) == 3){
			// printf("%s %s %d\n", cacheLevel, input, val);
			if(strcmp(cacheLevel, "L1") == 0){
				if (strcmp(input, "block_size") 		== 0){
					l1_data->block_size = val;
					l1_inst->block_size = val;
				} else if (strcmp(input, "cache_size") 	== 0){
					l1_data->cache_size = val;
					l1_inst->cache_size = val;
				} else if (strcmp(input, "assoc") 		== 0){
					l1_data->assoc = val;
					l1_inst->assoc = val;
				} else if (strcmp(input, "hit_time") 	== 0){
					l1_data->hit_time = val;
					l1_inst->hit_time = val;
				} else if (strcmp(input, "miss_time") 	== 0){
					l1_data->miss_time = val;
					l1_inst->miss_time = val;
				}				
			} else if (strcmp(cacheLevel, "L2") == 0){
				if (strcmp(input, "block_size") == 0){
					l2->block_size = val;
				} else if (strcmp(input, "cache_size") 		== 0){
					l2->cache_size = val;
				} else if (strcmp(input, "assoc") 			== 0){
					l2->assoc = val;
				} else if (strcmp(input, "hit_time") 		== 0){
					l2->hit_time = val;
				} else if (strcmp(input, "miss_time") 		== 0){
					l2->miss_time = val;
				} else if (strcmp(input, "transfer_time") 	== 0){
					l2->transfer_time = val;
				} else if (strcmp(input, "bus_width") 		== 0){
					l2->bus_width = val;
				}
			} else if (strcmp(cacheLevel, "mm") == 0){
				if (strcmp(input, "mem_sendaddr") 			== 0){
					main_mem->mem_sendaddr = val;
				} else if (strcmp(input, "mem_ready") 		== 0){
					main_mem->mem_ready = val;
				} else if (strcmp(input, "mem_chunktime") 	== 0){
					main_mem->mem_chunktime = val;
				} else if (strcmp(input, "mem_chunksize") 	== 0){
					main_mem->mem_chunksize = val;
				}
			}
		}

		uint addres_length = 64;

		l1_data->num_sets = l1_data->cache_size / (l1_data->assoc * l1_data->block_size);
		l1_data->tag_size = addres_length - log(l1_data->num_sets)/log(2) - log(l1_data->block_size)/log(2);
		l1_data->next_level = l2;

		l1_inst->num_sets = l1_inst->cache_size / (l1_inst->assoc * l1_inst->block_size);
		l1_inst->tag_size = addres_length - log(l1_data->num_sets)/log(2) - log(l1_inst->block_size)/log(2);
		l1_inst->next_level = l2;

		l2->num_sets = l2->cache_size / (l2->assoc * l2->block_size);
		l2->tag_size = addres_length - log(l1_data->num_sets)/log(2) - log(l2->block_size)/log(2);
		l2->next_level = main_mem;

		main_mem->next_level = NULL;
	}

	return 0;
}

void allocate_blocks(cache* l1_data, cache* l1_inst, cache* l2){
	uint i = 0;
	uint j = 0;
	l1_data->cache_set = malloc(l1_data->num_sets * sizeof(cache_set*));
	for(i = 0; i < l1_data->num_sets; i++){
		l1_data->cache_set[i] = malloc(l1_data->assoc * sizeof(cache_set));
		for(j = 0; j < l1_data->assoc; j++){
			l1_data->cache_set[i][j].valid = 0;
			l1_data->cache_set[i][j].dirty = 0;
			l1_data->cache_set[i][j].lru = LRU_Construct(l1_data->assoc);
		}
	}
	l1_inst->cache_set = malloc(l1_inst->num_sets * sizeof(cache_set*));
	for(i = 0; i < l1_inst->num_sets; i++){
		l1_inst->cache_set[i] = malloc(l1_inst->assoc * sizeof(cache_set));
		for(j = 0; j < l1_inst->assoc; j++){
			l1_inst->cache_set[i][j].valid = 0;
			l1_inst->cache_set[i][j].dirty = 0;
			l1_inst->cache_set[i][j].lru = LRU_Construct(l1_inst->assoc);
		}
	}
	l2->cache_set = malloc(l2->num_sets * sizeof(cache_set*));
	for(i = 0; i < l2->num_sets; i++){
		l2->cache_set[i] = malloc(l2->assoc * sizeof(cache_set));
		for(j = 0; j < l2->assoc; j++){
			l2->cache_set[i][j].valid = 0;
			l2->cache_set[i][j].dirty = 0;
			l2->cache_set[i][j].lru = LRU_Construct(l2->assoc);
		}
	}
}

void read_trace(cache* l1_data, cache* l1_inst, ull* num_inst, ull* num_reads, ull* num_writes){
	char op;
	unsigned long long int address = 0;
	// printf("%lu\n", sizeof(unsigned long long int));
	uint bytesize = 0;
	while(scanf("%c %llx %d\n", &op, &address, &bytesize) == 3){
		// printf("%c %llx %d\n", op, address, bytesize);
		if(op == 'I'){
			*num_inst = *num_inst + 1;
		} else if (op == 'R'){
			*num_reads = *num_reads + 1;
		} else if (op == 'W'){
			*num_writes = *num_writes + 1;
		}
		look_through_cache(l1_inst, address, op);
		if((*num_inst + *num_reads + *num_writes) % 380000 == 0){
			//must flush and invalidate all caches
		}
	}
	#ifdef DEBUG
		printf("%llu \n", *num_inst);
		printf("%llu \n", *num_reads);
		printf("%llu \n", *num_writes);
	#endif 
}

void look_through_cache(cache* cache_level, ulli address, char type){
	
	uint i;
	// uint new_address = 
	// struct cache_block block;

	if (cache_level->next_level != NULL){
		ulli index, tag;
		tag 	= (address >> (64 - cache_level->tag_size));
		index 	= address << cache_level->tag_size;
		index 	= index   >> (cache_level->tag_size);
		index 	= index   >> (uint)(log(cache_level->block_size)/log(2));
		#ifdef DEBUG
			printf("sizes: %u %u\n", cache_level->tag_size, cache_level->num_sets);
			printf("address: %llx %llu\n", tag, index);
		#endif	

		for(i = 0; i < cache_level->assoc; i++){
			if(cache_level->cache_set[index][i].valid == true && cache_level->cache_set[index][i].tag == tag){
				//We found a match, and it's valid! lets count it as a hit!
				cache_level->num_hits = cache_level->num_hits + 1;
				if(type == 'W'){
					cache_level->cache_set[index][i].dirty = true;
				}
				return;
			}
		}
		
		//didn't find the stuff, def a miss
		cache_level->num_misses = cache_level->num_misses + 1;
		//must fetch the data from the next level in the cache, I don't think we need this function.  should be able to do this based off the recursiveness.
		//need to return the tag, index, assoc_level from the next level.  and put the values from that into the first cache/set it to valid
		fetch_from_next_cache(cache_level->next_level, tag, index, i - 1);

	// 	//Recursive search through the cache, not in main memory
	 	look_through_cache(cache_level->next_level, address, type);
	 	LRU_Update(cache_level, index, i - 1);
	 	return;
		//We returned the block, now update the block using an LRU

		/*TODO!!*/
	}
	// printf("2\n");

	//We are in main memory
	cache_level->num_hits = cache_level->num_hits + 1;
	return;
}

void fetch_from_next_cache(cache* next_level, ulli tag, ulli index, uint assoc_level){
	if(next_level->next_level == NULL)
		return;

}

LRU* LRU_Construct(unsigned int num_block)
{
	if (num_block)
	{
		struct LRU* lru = (struct LRU*)malloc( sizeof(struct LRU));

		struct node* n_ptr = NULL;

		 for (unsigned int i = 0; i < num_block; i++)
		 {
		 	n_ptr = (struct node*)malloc( sizeof(struct node));
			if(i) {
				struct node* l_ptr = n_ptr;
				l_ptr--;
				l_ptr->next = n_ptr;
			}

			if (!i) lru->head = n_ptr;
			if (i == num_block-1) lru->tail = n_ptr;

			n_ptr++;
		 }
		return lru;
	}
	return NULL;
}

void LRU_Update(cache* cache_level, uint set, uint assoc_level){
	// printf("%u %u\n", set, index);
	if (!assoc_level)
	{
		return;
	}

	struct node* cur_ptr;
	struct node* i_ptr;

	cur_ptr = cache_level->cache_set[set][assoc_level].lru->head;

	for (unsigned int i = 0; i < assoc_level - 1; i++)
	{
		cur_ptr = cur_ptr->next;
		if (!cur_ptr && !cur_ptr->next) return; //Bad error checking
	}

	i_ptr = cur_ptr->next;

	if (cur_ptr->next) 
	{
		cur_ptr = cur_ptr->next->next;
	}

	i_ptr->next = cache_level->cache_set[set][assoc_level].lru->head;
	cache_level->cache_set[set][assoc_level].lru->head = i_ptr;

	while(cur_ptr->next) cur_ptr = cur_ptr->next;

	cache_level->cache_set[set][assoc_level].lru->tail = cur_ptr;
}

node* LRU_getLRU(struct LRU *lru){
	struct node* cur_ptr = lru->head;
	while(cur_ptr->next != NULL){
		cur_ptr = cur_ptr->next;
	}
	return cur_ptr;
}


void report(cache* l1_data, cache* l1_inst, cache* l2, cache* main_mem, ull* num_inst, ull* num_reads, ull* num_writes){

	// FILE * outputFile;

	// outputFile = fopen("results.dat", "w+");

	// //Calculate the l1_inst things
	// l1_inst->total_requests = l1_inst->num_hits + l1_inst->num_misses;
 //    l1_inst->hit_rate 	=	(double) l1_inst->num_hits 		/ 	l1_inst->total_requests 	* 100;
 //    l1_inst->miss_rate 	= 	(double) l1_inst->num_misses 	/ 	l1_inst->total_requests 	* 100;

 //    //Calculate the l1_data things
 //    l1_data->total_requests = l1_data->num_hits + l1_data->num_misses;
 //    l1_data->hit_rate 	=	(double) l1_data->num_hits 		/ 	l1_data->total_requests 	* 100;
 //    l1_data->miss_rate 	= 	(double) l1_data->num_misses 	/ 	l1_data->total_requests 	* 100;

 //    //Calculate the l2 things
 //    l2->total_requests = l2->num_hits + l2->num_misses;
 //    l2->hit_rate 		=	(double) l2->num_hits 			/ 	l2->total_requests 		* 100;
 //    l2->miss_rate 		= 	(double) l2->num_misses 		/ 	l2->total_requests 		* 100;

 //    //Calculate percent of reference types
 //    ull total_traces 		= *num_inst + *num_reads + *num_writes;
 //    double inst_percent 	= *num_inst 	/ total_traces 	* 100;
 //    double read_precent 	= *num_reads 	/ total_traces 	* 100;
 //    double write_percent 	= *num_writes 	/ total_traces 	* 100;

	// fprintf(outputFile, "Memory System: \n");
	// fprintf(outputFile, "	Dcache size = %u  :  ways = %u  :  block size = %u \n", l1_data->cache_size, l1_data->assoc, l1_data->block_size);
	// fprintf(outputFile, "	Icache size = %u  :  ways = %u  :  block size = %u \n", l1_inst->cache_size, l1_inst->assoc, l1_inst->block_size);
	// fprintf(outputFile, "	L2-cache size = %u  :  ways = %u  :  block size = %u \n", l2->cache_size, l2->assoc, l2->block_size);
	// fprintf(outputFile, "	Memory ready time = %u  :  chunksize = %u  :  chunktime = %u \n", main_mem->mem_ready, main_mem->mem_chunksize, main_mem->mem_chunktime);
	// fprintf(outputFile, "\n");

	// fprintf(outputFile, "Execute time 	=	%llu;		Total refs	= %llu\n", , l1_inst->total_requests + l1_data->total_requests);
	// fprintf(outputFile, "Flush time 		=	%llu", );
	// fprintf(outputFile, "Inst refs 		=	%llu; 	Data refs 	= %llu", l1_inst->total_requests, l1_data->total_requests);
	// fprintf(outputFile, "\n");

	// fprintf(outputFile, "Number of reference types : 	[Percentage]\n");
	// fprintf(outputFile, "	Reads 	= 	%llu 			[%.1f%%]\n", *num_reads, 	read_precent);
	// fprintf(outputFile, "	Writes 	= 	%llu 			[%.1f%%]\n", *num_writes, 	write_percent);
	// fprintf(outputFile, "	Inst. 	= 	%llu			[%.1f%%]\n", *num_inst, 	inst_percent);
	// fprintf(outputFile, "	Total 	= 	%llu\n ", total_traces);
	// fprintf(outputFile, "\n");

	// fprintf(outputFile, "Total cycles for activities: 	[Percentage]\n");
	// fprintf(outputFile, "	Reads 	= 	%llu 			[%.1f%%]\n", , );
	// fprintf(outputFile, "	Writes 	= 	%llu 			[%.1f%%]\n", , );
	// fprintf(outputFile, "	Inst. 	= 	%llu 			[%.1f%%]\n", , );
	// fprintf(outputFile, "	Total 	= 	%llu\n ", );
	// fprintf(outputFile, "\n");

	// fprintf(outputFile, "Average cycles per activity:\n");
	// fprintf(outputFile, "	Read = %d; Write = %d; Inst. = %d\n", , , );
	// fprintf(outputFile, "Ideal: Exec. Time = %llu; CPI = %d\n", , );
	// fprintf(outputFile, "Ideal mis-aligned: Exec. Time = %llu; CPI = %d\n", , );
	// fprintf(outputFile, "\n");

	// fprintf(outputFile, "Memory Level: 	L1i");
	// fprintf(outputFile, "	Hit Count = %llu 		Miss Count = %llu\n", l1_inst->num_hits, l1_inst->num_misses);
	// fprintf(outputFile, "	Total Requests = %llu", l1_inst->total_requests);
	// fprintf(outputFile, " 	Hit Rate = %.1f%% 	Miss Rate = %.1f%%\n", l1_inst->hit_rate, l1_inst->miss_rate);
	// fprintf(outputFile, "	Kickouts = %llu; Dirty Kickouts = %llu; Transfers = %llu\n", l1_inst->kickouts, l1_inst->dirty_kickouts, l1_inst->transfers);
	// fprintf(outputFile, "Flush Kickouts = %llu\n", l1_inst->flush_kickouts);
	// fprintf(outputFile, "\n");

	// fprintf(outputFile, "Memory Level: 	L1d");
	// fprintf(outputFile, "	Hit Count = %llu 		Miss Count = %llu\n", l1_data->num_hits, l1_data->num_misses);
	// fprintf(outputFile, "	Total Requests = %llu", l1_data->total_requests);
	// fprintf(outputFile, " 	Hit Rate = %.1f%% 	Miss Rate = %.1f%%\n", l1_data->hit_rate, l1_data->miss_rate);
	// fprintf(outputFile, "	Kickouts = %llu; Dirty Kickouts = %llu; Transfers = %llu\n", l1_data->kickouts, l1_data->dirty_kickouts, l1_data->transfers);
	// fprintf(outputFile, "Flush Kickouts = %llu\n", l1_data->flush_kickouts);
	// fprintf(outputFile, "\n");

	// fprintf(outputFile, "Memory Level: 	L2");
	// fprintf(outputFile, "	Hit Count = %llu 		Miss Count = %llu\n", l2->num_hits, l2->num_misses);
	// fprintf(outputFile, "	Total Requests = %llu", l2->total_requests);
	// fprintf(outputFile, " 	Hit Rate = %.1f%% 	Miss Rate = %.1f%%\n", l2->hit_rate, l2->miss_rate);
	// fprintf(outputFile, "	Kickouts = %llu; Dirty Kickouts = %llu; Transfers = %llu\n", l2->kickouts, l2->dirty_kickouts, l2->transfers);
	// fprintf(outputFile, "Flush Kickouts = %llu\n", l2->flush_kickouts);
	// fprintf(outputFile, "\n");

	// fprintf(outputFile, "L1 cache cost (Icache $%d) + (Dcache $%d) = $%d", , , );
	// fprintf(outputFile, "L1 cache cost = $%d; Memory cost = $%d; Total cost = $%d", , , );
	// fprintf(outputFile, "Flushes = %f  :  Invalidates = %f", , );

	// fclose(outputFile);
}








