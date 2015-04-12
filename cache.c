#include "cache.h"
#include "LRU.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

 //#define DEBUG

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

		uint address_length = 64;

		l1_data->num_sets = l1_data->cache_size / (l1_data->assoc * l1_data->block_size);
		l1_data->tag_size = address_length - log(l1_data->num_sets)/log(2) - log(l1_data->block_size)/log(2);
		l1_data->next_level = l2;

		l1_inst->num_sets = l1_inst->cache_size / (l1_inst->assoc * l1_inst->block_size);
		l1_inst->tag_size = address_length - log(l1_data->num_sets)/log(2) - log(l1_inst->block_size)/log(2);
		l1_inst->next_level = l2;

		l2->num_sets = l2->cache_size / (l2->assoc * l2->block_size);
		l2->tag_size = address_length - log(l2->num_sets)/log(2) - log(l2->block_size)/log(2);
		l2->next_level = main_mem;

		main_mem->next_level = NULL;
	}

	return 0;
}

void allocate_blocks(cache* l1_data, cache* l1_inst, cache* l2){
	// l1_data->cache_set = malloc(l1_data->num_sets * sizeof(cache_set));
	// l1_inst->cache_set = malloc(l1_inst->num_sets * sizeof(cache_set));
	// l2     ->cache_set = malloc(l2->num_sets      * sizeof(cache_set));

	cache_alloc(l1_data);
	cache_alloc(l1_inst);
	cache_alloc(l2);
}

void cache_alloc(cache* cache_level)
{
	cache_level->cache_set = malloc(cache_level->num_sets * sizeof(cache_set));
	uint i = 0;
	uint j = 0;

	//For every cache set, malloc all blocks
	for(i = 0; i < cache_level->num_sets; i++){
		//Construct an lru and return the pointer 
		cache_level->cache_set[i].lru = LRU_Construct(cache_level->assoc);
		//Malloc the blocks at the index
		cache_level->cache_set[i].block = malloc(cache_level->assoc * sizeof(cache_block));

		for (j = 0; j < cache_level->assoc; j++)
		{
			//Set the valid and dirty bits
			cache_level->cache_set[i].block[j].valid = 0;
			cache_level->cache_set[i].block[j].dirty = 0;
		}
	}
}
//recreates the address
ulli create_address(cache* cache_level, ulli tag, ulli index, ulli byte_offset){
	ulli address = 0;
	address |= (tag << (64 - cache_level->tag_size));
	address |= (index << (uint)(log(cache_level->block_size)/log(2)));
	address |= byte_offset;
	return address;
}

void prep_search_cache(cache* cache_level, ulli address, uint bytesize, char op){
	ulli index;
	//find out how many times we need to 
	uint num_i = num_indices(cache_level, address, bytesize);
	index = get_index(cache_level, address);
	for(uint i = 0; i < num_i; i++){
		if(i > 0){
			//if we go onto another index, we need to recreate the address, and the byteoffset will be 0, since we start at the beginning of the next index
			bytesize = (get_byte_offset(cache_level, address) + bytesize) - cache_level->block_size;
			address = create_address(cache_level, get_tag(cache_level, address), index, 0);
		}
		// search_cache(cache_level, address, op, bytesize, index);
		if(search_cache(cache_level, address, op, bytesize, index) == false){
			index = get_index(cache_level->next_level, address);
			// if we get a miss, we need to go to the second level cache
			search_cache(cache_level->next_level, address, op, bytesize, index);
			// still need to implement the main memory look through here.
			// can just check if it's a miss.
			// --------
			// Will need to determine how many cycles it takes based off the chunksize of the memory
		}
		index++;
	}
}

void read_trace(cache* l1_data, cache* l1_inst, cache* l2, ull* num_inst, ull* num_reads, ull* num_writes){
	char op;
	unsigned long long int address = 0;
	uint bytesize = 0;
	
	while(scanf("%c %llx %d\n", &op, &address, &bytesize) == 3){
		 // printf("%c %llx %d\n", op, address, bytesize);
		
		if(op == 'I'){
			*num_inst = *num_inst + 1;
			prep_search_cache(l1_inst, address, bytesize, op);
		} else if (op == 'R'){
			*num_reads = *num_reads + 1;
			prep_search_cache(l1_data, address, bytesize, op);
		} else if (op == 'W'){
			*num_writes = *num_writes + 1;
			prep_search_cache(l1_data, address, bytesize, op);
		}
		
		if((*num_inst + *num_reads + *num_writes) % 380000 == 0){
			//write all dirty blocks to the next level of cache.  do this all the way down to main memory
			//Currently, this flushes l1_data, then l2, then l1_inst, then l2
			flush(l1_data);
			flush(l1_inst);
		}
	}
}

void flush(cache* cache_level)
{
	uint i;
	for (i = 0; i < cache_level->num_sets; i++)
	{
		uint j;
		for (j = 0; j < cache_level->assoc; j++)
		if (cache_level->cache_set[i].block[j].dirty)
		{
			//Write this through, increment kickouts
			cache_level->dirty_kickouts++;

			ulli dirty_addr = (cache_level->cache_set[i].block[j].tag << (64 -cache_level->tag_size));
			dirty_addr |= i << cache_level->block_size;

			cache_level->dirty_kickouts = cache_level->dirty_kickouts + 1;
			look_through_cache(cache_level->next_level, dirty_addr, 'W', 0, i);
		}
	}
}

bool search_cache(cache* cache_level, ulli address, char type, ulli num_bytes, ulli index){
	if (cache_level->next_level != NULL){
		ulli tag, byte_offset;
		tag 		= get_tag(cache_level, address);
		byte_offset = get_byte_offset(cache_level, address);
		// if(cache_level->next_level->next_level == NULL){
		// 	index 		= get_index(cache_level, address);
		// }
		uint num_refs = 0;
		uint word_offset = byte_offset % 4;
		if(word_offset + num_bytes > 4 && cache_level->next_level->next_level != NULL){
			uint i = byte_offset;
			uint j = 0;
			while(i < cache_level->block_size && j < (num_bytes + word_offset)){
				num_refs++;
				i += 4;
				j += 4;
			}
		}else{
			num_refs = 1;
		}

		for(uint i = 0; i < cache_level->assoc; i++){
			if(cache_level->cache_set[index].block[i].valid == true && cache_level->cache_set[index].block[i].tag == tag){
				cache_level->num_hits = cache_level->num_hits + num_refs;
				if(type == 'W'){
					cache_level->cache_set[index].block[i].dirty = true;
				}
				return true;
			}
		}

		cache_level->num_misses = cache_level->num_misses + 1;
		cache_level->num_hits = cache_level->num_hits + num_refs - 1;

		//ulli new_index = get_index(cache_level->next_level, address);
		// search_cache(cache_level->next_level, address, type, num_bytes, index);
		uint b = LRU_Get_LRU(cache_level, index);
	 	LRU_Update(cache_level, index, b);


	 	if(cache_level->cache_set[index].block[b].dirty == true){
	 		cache_level->cache_set[index].block[b].dirty = false;
	 		//write through to next level, dirty kickout of a block
	 		//Same index as current address, also need to extract tag and reconstruct address to pass
	 		ulli dirty_addr = address;

	 		//Clear the tag, and replace with the dirty tag.  Also, clear the byte index?
	 		dirty_addr &= (0xFFFFFFFFFFFFFFFF >> cache_level->tag_size);
	 		dirty_addr |= (cache_level->cache_set[index].block[b].tag << (64 -cache_level->tag_size));

	 		//CLEAR BYTE INDEX
	 		dirty_addr &= (0xFFFFFFFFFFFFFFFF << (uint)(log(cache_level->block_size)/log(2)));

	 		cache_level->dirty_kickouts = cache_level->dirty_kickouts + 1;
	 		///HAHA GODDAMMIT.  im calling the wrong function, but it works
	 		//will definitely need to fix this.....
	 		look_through_cache(cache_level->next_level, dirty_addr, 'W', 0, index);
	 	}


		cache_level->cache_set[index].block[b].tag 		= tag;
	 	cache_level->cache_set[index].block[b].valid 	= true;
	 	cache_level->cache_set[index].block[b].dirty 	= false;
	 	return false;
	 } else{
	 	cache_level->num_hits = cache_level->num_hits + 1;
		return true;
	 }
}

void look_through_cache(cache* cache_level, ulli address, char type, ulli num_bytes, ulli index){
	uint i;
	// printf("num_bytes: %llu\n", num_bytes);
	if (cache_level->next_level != NULL){

		ulli tag, byte_offset;
		tag 		= get_tag(cache_level, address);
		byte_offset = get_byte_offset(cache_level, address);
		if(cache_level->next_level->next_level == NULL){
			index 		= get_index(cache_level, address);
			if(byte_offset + num_bytes > cache_level->block_size){
				printf("block_size: %u\n", cache_level->block_size);
				printf("total: %llu\n", byte_offset + num_bytes);
				printf("byte_offset: %llu\n", byte_offset);
				printf("num_bytes: %llu\n", num_bytes);
			}
		}
		
		uint num_refs = 0;
		uint word_offset = byte_offset % 4;
		if(word_offset + num_bytes > 4 && cache_level->next_level->next_level != NULL){
			// printf("byte_offset: %llu\n", byte_offset);
			// printf("word_offset: %u\n", word_offset);
			uint i = byte_offset;
			uint j = 0;
			//byte_offset
			while(i < cache_level->block_size && j < (num_bytes + word_offset)){//(num_bytes + word_offset)){
				num_refs++;
				i += 4;
				j += 4;
			}
			// printf("num_refs: %u\n", num_refs);
		}else{
			num_refs = 1;
		}
		
		for(i = 0; i < cache_level->assoc; i++){
			#ifdef DEBUG
				// printf("%llu cache_level \n", cache_level->num_sets);
				// printf("%llu index \n\n", index);
			#endif

			if(cache_level->cache_set[index].block[i].valid == true && cache_level->cache_set[index].block[i].tag == tag){
				//We found a match, and it's valid! lets count it as a hit!
				// printf("hit\n");
				cache_level->num_hits = cache_level->num_hits + num_refs;
				if(type == 'W'){
					cache_level->cache_set[index].block[i].dirty = true;
				}
				return;
			}
		}
		
		//didn't find the stuff, def a miss
		cache_level->num_misses = cache_level->num_misses + 1;

		cache_level->num_hits = cache_level->num_hits + num_refs - 1;
	 	//Recursive search through the cache, not in main memory
	 	look_through_cache(cache_level->next_level, address, type, num_bytes, index);
	 	unsigned int b = LRU_Get_LRU(cache_level, index);
	 	LRU_Update(cache_level, index, b);

	 	if(cache_level->cache_set[index].block[b].dirty == true){
	 		cache_level->cache_set[index].block[b].dirty = false;
	 		//write through to next level, dirty kickout of a block
	 		//Same index as current address, also need to extract tag and reconstruct address to pass
	 		ulli dirty_addr = address;

	 		//Clear the tag, and replace with the dirty tag.  Also, clear the byte index?
	 		dirty_addr &= (0xFFFFFFFFFFFFFFFF >> cache_level->tag_size);
	 		dirty_addr |= (cache_level->cache_set[index].block[b].tag << (64 -cache_level->tag_size));

	 		//CLEAR BYTE INDEX
	 		dirty_addr &= (0xFFFFFFFFFFFFFFFF << (uint)(log(cache_level->block_size)/log(2)));

	 		cache_level->dirty_kickouts = cache_level->dirty_kickouts + 1;
	 		look_through_cache(cache_level->next_level, dirty_addr, 'W', 0, index);
	 	}

	 	cache_level->cache_set[index].block[b].tag 		= tag;
	 	cache_level->cache_set[index].block[b].valid 	= true;
	 	cache_level->cache_set[index].block[b].dirty 	= false;

	 	return;
	} 
	//We are in main memory
	cache_level->num_hits = cache_level->num_hits + 1;
	return;
}

ulli get_tag(cache* cache_level, ulli address){
	return (address >> (64 - cache_level->tag_size));
}

ulli get_index(cache* cache_level, ulli address){
	ulli index;
	index 	= address << cache_level->tag_size;
	index 	= index   >> (cache_level->tag_size);
	index 	= index   >> (uint)(log(cache_level->block_size)/log(2));
	return index;
}

ulli get_byte_offset(cache* cache_level, ulli address){
	ulli byte_offset;
	byte_offset = address << (64 - (uint)(log(cache_level->block_size)/log(2)));
	byte_offset = byte_offset >> (64 - (uint)(log(cache_level->block_size)/log(2)));
	return byte_offset;
}

//finds the number of indexes we need to use for a single instruction
int num_indices(cache* cache_level, ulli address, uint num_bytes){
	ulli index, tag, byte_offset;
	tag 		= get_tag(cache_level, address);
	index 		= get_index(cache_level, address);
	byte_offset = get_byte_offset(cache_level, address);

	int num_blocks_requested = 1;
	int j = (byte_offset + num_bytes);
	if(j > cache_level->block_size){
		// printf("byte_offset: %llu num_bytes: %u\n", byte_offset, num_bytes);
	}
	while(j > cache_level->block_size){
		j = j - cache_level->block_size;
		num_blocks_requested++;
	}
	if(num_blocks_requested > 1){
		// printf("%d\n", num_blocks_requested);
	}

	return num_blocks_requested;
}

void report(cache* l1_data, cache* l1_inst, cache* l2, cache* main_mem, ull* num_inst, ull* num_reads, ull* num_writes){

	FILE * outputFile;

	outputFile = fopen("results.dat", "w+");

	//Calculate the l1_inst things
	l1_inst->total_requests = l1_inst->num_hits + l1_inst->num_misses;
    l1_inst->hit_rate 	=	(double) l1_inst->num_hits 		/ 	l1_inst->total_requests 	* 100;
    l1_inst->miss_rate 	= 	(double) l1_inst->num_misses 	/ 	l1_inst->total_requests 	* 100;

    //Calculate the l1_data things
    l1_data->total_requests = l1_data->num_hits + l1_data->num_misses;
    l1_data->hit_rate 	=	(double) l1_data->num_hits 		/ 	l1_data->total_requests 	* 100;
    l1_data->miss_rate 	= 	(double) l1_data->num_misses 	/ 	l1_data->total_requests 	* 100;

    //Calculate the l2 things
    l2->total_requests = l2->num_hits + l2->num_misses;
    l2->hit_rate 		=	(double) l2->num_hits 			/ 	l2->total_requests 		* 100;
    l2->miss_rate 		= 	(double) l2->num_misses 		/ 	l2->total_requests 		* 100;

    //Calculate percent of reference types
    ull total_traces 		= *num_inst + *num_reads + *num_writes;
    double inst_percent 	= ((double)*num_inst 	/ (double)total_traces) * 100;
    double read_precent 	= ((double)*num_reads 	/ (double)total_traces)	* 100;
    double write_percent 	= ((double)*num_writes 	/ (double)total_traces) * 100;

	fprintf(outputFile, "Memory System: \n");
	fprintf(outputFile, "	Dcache size = %u  :  ways = %u  :  block size = %u \n", l1_data->cache_size, l1_data->assoc, l1_data->block_size);
	fprintf(outputFile, "	Icache size = %u  :  ways = %u  :  block size = %u \n", l1_inst->cache_size, l1_inst->assoc, l1_inst->block_size);
	fprintf(outputFile, "	L2-cache size = %u  :  ways = %u  :  block size = %u \n", l2->cache_size, l2->assoc, l2->block_size);
	fprintf(outputFile, "	Memory ready time = %u  :  chunksize = %u  :  chunktime = %u \n", main_mem->mem_ready, main_mem->mem_chunksize, main_mem->mem_chunktime);
	fprintf(outputFile, "\n");

	// fprintf(outputFile, "Execute time 	=	%llu;		Total refs	= %llu\n", , l1_inst->total_requests + l1_data->total_requests);
	// fprintf(outputFile, "Flush time 		=	%llu", );
	// fprintf(outputFile, "Inst refs 		=	%llu; 	Data refs 	= %llu", l1_inst->total_requests, l1_data->total_requests);
	// fprintf(outputFile, "\n");

	fprintf(outputFile, "Number of reference types : 	[Percentage]\n");
	fprintf(outputFile, "	Reads 	= 	%llu 	[%.1f%%]\n", *num_reads, 	read_precent);
	fprintf(outputFile, "	Writes 	= 	%llu 	[%.1f%%]\n", *num_writes, 	write_percent);
	fprintf(outputFile, "	Inst. 	= 	%llu	[%.1f%%]\n", *num_inst, 	inst_percent);
	fprintf(outputFile, "	Total 	= 	%llu\n ", total_traces);
	fprintf(outputFile, "\n");

	// fprintf(outputFile, "Total cycles for activities: 	[Percentage]\n");
	// fprintf(outputFile, "	Reads 	= 	%llu 	[%.1f%%]\n", , );
	// fprintf(outputFile, "	Writes 	= 	%llu 	[%.1f%%]\n", , );
	// fprintf(outputFile, "	Inst. 	= 	%llu 	[%.1f%%]\n", , );
	// fprintf(outputFile, "	Total 	= 	%llu\n ", );
	// fprintf(outputFile, "\n");

	fprintf(outputFile, "Average cycles per activity:\n");
	// fprintf(outputFile, "	Read = %d; Write = %d; Inst. = %d\n", , , );
	fprintf(outputFile, "Ideal: Exec. Time = %llu; CPI = %.1f\n", total_traces + *num_inst, round(10*(((double)total_traces + (double)*num_inst)/ (double)*num_inst))/10);
	fprintf(outputFile, "Ideal mis-aligned: Exec. Time = %llu; CPI = %.1f\n", l1_inst->total_requests + l1_data->total_requests + *num_inst, round(10*((double)l1_inst->total_requests + (double)l1_data->total_requests + (double)*num_inst)/ (double)*num_inst)/10);
	fprintf(outputFile, "\n");

	fprintf(outputFile, "Memory Level: 	L1i\n");
	fprintf(outputFile, "	Hit Count = %llu 		Miss Count = %llu\n", l1_inst->num_hits, l1_inst->num_misses);
	fprintf(outputFile, "	Total Requests = %llu\n", l1_inst->total_requests);
	fprintf(outputFile, " 	Hit Rate = %.1f%% 	Miss Rate = %.1f%%\n", l1_inst->hit_rate, l1_inst->miss_rate);
	fprintf(outputFile, "	Kickouts = %llu; Dirty Kickouts = %llu; Transfers = %llu\n", l1_inst->kickouts, l1_inst->dirty_kickouts, l1_inst->transfers);
	fprintf(outputFile, "Flush Kickouts = %llu\n", l1_inst->flush_kickouts);
	fprintf(outputFile, "\n");

	fprintf(outputFile, "Memory Level: 	L1d\n");
	fprintf(outputFile, "	Hit Count = %llu 		Miss Count = %llu\n", l1_data->num_hits, l1_data->num_misses);
	fprintf(outputFile, "	Total Requests = %llu\n", l1_data->total_requests);
	fprintf(outputFile, " 	Hit Rate = %.1f%% 	Miss Rate = %.1f%%\n", l1_data->hit_rate, l1_data->miss_rate);
	fprintf(outputFile, "	Kickouts = %llu; Dirty Kickouts = %llu; Transfers = %llu\n", l1_data->kickouts, l1_data->dirty_kickouts, l1_data->transfers);
	fprintf(outputFile, "Flush Kickouts = %llu\n", l1_data->flush_kickouts);
	fprintf(outputFile, "\n");

	fprintf(outputFile, "Memory Level: 	L2\n");
	fprintf(outputFile, "	Hit Count = %llu 		Miss Count = %llu\n", l2->num_hits, l2->num_misses);
	fprintf(outputFile, "	Total Requests = %llu\n", l2->total_requests);
	fprintf(outputFile, " 	Hit Rate = %.1f%% 	Miss Rate = %.1f%%\n", l2->hit_rate, l2->miss_rate);
	fprintf(outputFile, "	Kickouts = %llu; Dirty Kickouts = %llu; Transfers = %llu\n", l2->kickouts, l2->dirty_kickouts, l2->transfers);
	fprintf(outputFile, "Flush Kickouts = %llu\n", l2->flush_kickouts);
	fprintf(outputFile, "\n");

	fprintf(outputFile, "Memory Level:  L1i\n");

	for(ulli i = 0; i < l1_inst->num_sets; i++){
		if(l1_inst->cache_set[i].block[0].valid == true){
			fprintf(outputFile, "Index: %llx | V:1 D:%d Tag: %llx\n", i, l1_inst->cache_set[i].block[0].dirty, l1_inst->cache_set[i].block[0].tag);
		}
	}
	fprintf(outputFile, "\n");
	fprintf(outputFile, "Memory Level:  L1d\n");

	for(ulli i = 0; i < l1_data->num_sets; i++){
		if(l1_data->cache_set[i].block[0].valid == true){
			fprintf(outputFile, "Index: %llx | V:1 D:%d Tag: %llx\n", i, l1_data->cache_set[i].block[0].dirty, l1_data->cache_set[i].block[0].tag);
		}
	}
	fprintf(outputFile, "\n");
	fprintf(outputFile, "Memory Level:  L2\n");

	for(ulli i = 0; i < l2->num_sets; i++){
		if(l2->cache_set[i].block[0].valid == true){
			fprintf(outputFile, "Index: %llx | V:1 D:%d Tag: %llx\n", i, l2->cache_set[i].block[0].dirty, l2->cache_set[i].block[0].tag);
		}
	}

	// fprintf(outputFile, "L1 cache cost (Icache $%d) + (Dcache $%d) = $%d", , , );
	// fprintf(outputFile, "L1 cache cost = $%d; Memory cost = $%d; Total cost = $%d", , , );
	// fprintf(outputFile, "Flushes = %f  :  Invalidates = %f", , );

	fclose(outputFile);
}
