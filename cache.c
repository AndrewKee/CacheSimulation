#include "cache.h"
#include "LRU.h"


 //#define DEBUG
// uint global_cnt;

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



		l1_inst->bus_width = 4;
		l1_data->bus_width = 4;
		main_mem->bus_width = 0;

		//Fully Associative
		if(l1_data->assoc == 0){
			l1_data->assoc = l1_data->cache_size / l1_data->block_size;
		}

		l1_data->num_sets = l1_data->cache_size / (l1_data->assoc * l1_data->block_size);
		l1_data->next_level = l2;

		//Fully Associative
		if(l1_inst->assoc == 0){
			l1_inst->assoc = l1_inst->cache_size / l1_inst->block_size;
		}
		
		l1_inst->num_sets = l1_inst->cache_size / (l1_inst->assoc * l1_inst->block_size);
		l1_inst->next_level = l2;

		//Fully Associative
		if(l2->assoc == 0){
			l2->assoc = l2->cache_size / l2->block_size;
		}

		l2->num_sets = l2->cache_size / (l2->assoc * l2->block_size);
		l2->next_level = main_mem;

		main_mem->next_level = NULL;
	}

	return 0;
}

void allocate_blocks(cache* l1_data, cache* l1_inst, cache* l2){
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

ulli pack_addr(cache* cache_level, ulli tag, ulli index)
{
	ulli offset_bytes = log(cache_level->block_size)/log(2);
	ulli tag_shift = log(cache_level->block_size * cache_level->cache_size)/log(2);

	ulli address = index << offset_bytes;
	address |= (tag << tag_shift);

	return address;
}

void unpack_addr(cache* cache_level, ulli address, ulli *tag, ulli *index, ulli *offset)
{
	ulli boffset = log(cache_level->block_size)/log(2);
	ulli bindex = log(cache_level->block_size * cache_level->cache_size)/log(2);
	ulli index_shift = bindex - boffset;

	*offset = address & (cache_level->block_size - 1);
	*tag = (address >> bindex);
	*index = (address >> boffset) & ~(0xFFFFFFFFFFFFFFFF << (index_shift -1));

	printf("OFFSET BITS : %llx\n", boffset);
	printf("INDEX BITS : %u\n", (uint)bindex);
	printf("TAG : %llx\n", *tag);
	printf("OFFSET : %llx\n", *offset);
	printf("INDEX : %llx\n", *index);	
	printf("MASK : %lx\n", ~(0xFFFFFFFFFFFFFFFF << (index_shift)));
}

ulli prep_search_cache(cache* cache_level, ulli address, uint bytesize, char op){
	ulli cycles = 0;

	while (bytesize > 0)
	{
		cycles += search_cache(cache_level, address, op);
		address += cache_level->bus_width;
		bytesize -= cache_level->bus_width;	
	}	

return cycles;
}

void read_trace(cache* l1_data, cache* l1_inst, cache* l2, results* cache_results){
	char op;
	unsigned long long int address = 0;
	uint bytesize = 0;
	unsigned long inst = 0;

	while(scanf("%c %llx %d\n", &op, &address, &bytesize) == 3){
		// printf("%c %llx %d\n", op, address, bytesize);
		if(op == 'I'){
			inst++;
			cache_results->num_inst = cache_results->num_inst + 1;
			cache_results->inst_time += prep_search_cache(l1_inst, address, bytesize, op);
		} else if (op == 'R'){
			cache_results->num_reads = cache_results->num_reads + 1;
			cache_results->read_time += prep_search_cache(l1_data, address, bytesize, op);
		} else if (op == 'W'){
			cache_results->num_writes = cache_results->num_writes + 1;
			cache_results->write_time += prep_search_cache(l1_data, address, bytesize, op);
		}

		if (inst > 380000){
			cache_results->flush_time = flush(l1_data);
			cache_results->flush_time = flush(l1_inst); //nothin is dirty, so invalidate everything
			cache_results->flush_time = flush(l2);
			cache_results->flush_cnt = cache_results->flush_cnt + 1;
			cache_results->num_invalid = cache_results->num_invalid + 1;
			inst = 0;
		}
	}
}

void init_cache(cache* cache_level){
	uint i;
	for (i = 0; i < cache_level->num_sets; i++){
		uint j;
		for (j = 0; j < cache_level->assoc; j++){
			cache_level->cache_set[i].block[j].valid = false;
			cache_level->cache_set[i].block[j].dirty = false;
			cache_level->cache_set[i].block[j].tag 	 = 0;
		}
	}
}

uint flush(cache* cache_level){
	uint cycles = 0;

	uint i;
	for (i = 0; i < cache_level->num_sets; i++)
	{
		uint j;
		for (j = 0; j < cache_level->assoc; j++){
			//if (cache_level->cache_set[i].block[j].dirty && cache_level->cache_set[i].block[j].valid)
			if (cache_level->cache_set[i].block[j].dirty)
			{
				int dirty_addr = pack_addr(cache_level, cache_level->cache_set[i].block[j].tag, i);
				cache_level->flush_kickouts = cache_level->flush_kickouts + 1;
				//cache_level->dirty_kickouts = cache_level->dirty_kickouts + 1;

				//IS FLUSHING RIGHT? WHO KNOWS
				cycles += transfer(cache_level);

				cycles += search_cache(cache_level->next_level, dirty_addr, 'W');
			}
			cache_level->cache_set[i].block[j].valid = false;
			cache_level->cache_set[i].block[j].dirty = false;
		}
	}

	return cycles;
}


//Return the number of cycles required to transfer a block downstream
uint transfer(cache* cache_level)
{
	uint cycles;

	//We are not going to main memory
	if (cache_level->next_level->bus_width)
	{
		cycles = cache_level->next_level->transfer_time * (cache_level->block_size / cache_level->next_level->bus_width);
	}
	else //We are going to main memory
	{
		cycles = cache_level->next_level->mem_sendaddr + cache_level->next_level->mem_ready + (cache_level->next_level->mem_chunktime * (cache_level->block_size / cache_level->next_level->mem_chunksize));
	}
	return cycles;
}

uint search_cache(cache* cache_level, ulli address, char type){
	uint cycles = 0;

	if (cache_level->next_level != NULL){
		ulli tag, index, offset;
		//tag 		= get_tag(cache_level, address);
		//index 		= get_index(cache_level, address);
		unpack_addr(cache_level, address, &tag, &index, &offset);
		printf("%llx : INDEX\n", index);

		//look for the tag in the cache
		for(uint i = 0; i < cache_level->assoc; i++){
			if(cache_level->cache_set[index].block[i].valid == true && cache_level->cache_set[index].block[i].tag == tag){
				cache_level->num_hits = cache_level->num_hits + 1;
				//If its a write, make it dirty
				if(type == 'W'){
					cache_level->cache_set[index].block[i].dirty = true;
				}

				cycles += cache_level->hit_time;
				return cycles;
			}
		}

		//didnt find in cache, it's a miss
		cache_level->num_misses = cache_level->num_misses + 1;

		//We know that we are going to have a read delay
		cycles += cache_level->miss_time;

		uint b = LRU_Get_LRU(cache_level, index);

		//check if we need to kickout
		if(cache_level->cache_set[index].block[b].valid == true){
			cache_level->kickouts++;

			//check if its dirty, push it through
		 	if(cache_level->cache_set[index].block[b].dirty == true){
		 		ulli dirty_addr = pack_addr(cache_level, cache_level->cache_set[index].block[b].tag, index);
		 		cache_level->dirty_kickouts = cache_level->dirty_kickouts + 1;
		 		cycles += search_cache(cache_level->next_level, dirty_addr, 'W');
		 		cycles += transfer(cache_level);
	 		}
	 	}

	 	cycles += search_cache(cache_level->next_level, address, 'R');
		cycles += transfer(cache_level);	//Going to need to transfer down a level because we missed
		//update LRU
	 	LRU_Update(cache_level, index, b);
	 	
	 	//bring the stuff into this cache
		cache_level->cache_set[index].block[b].tag 		= tag;
	 	cache_level->cache_set[index].block[b].valid 	= true;
	 	//cache_level->cache_set[index].block[b].address 	= address;

	 	// if(type == 'W' && cache_level->next_level->next_level != NULL)
	 	// 	cache_level->cache_set[index].block[b].dirty 	= true;
	 	// else 
	 	// 	cache_level->cache_set[index].block[b].dirty 	= false;
	 	if (type == 'W'){
			cache_level->cache_set[index].block[b].dirty 	= true;
		}
		else{
		 	cache_level->cache_set[index].block[b].dirty 	= false;
		}

	 //We are in main memory 
	 }else
	 {
	 	cache_level->num_hits = cache_level->num_hits + 1;
	 }
	 //Must also add hit time, "replay"
	 cycles += cache_level->hit_time;
	 return cycles;
}

void report(cache* l1_data, cache* l1_inst, cache* l2, cache* main_mem, results* cache_results){

	FILE * outputFile;

	outputFile = fopen("results.dat", "wb");

	//Calculate the l1_inst things
	l1_inst->total_requests = l1_inst->num_hits + l1_inst->num_misses;
    l1_inst->hit_rate 	=	(double) l1_inst->num_hits 		/ 	l1_inst->total_requests 	* 100;
    l1_inst->miss_rate 	= 	(double) l1_inst->num_misses 	/ 	l1_inst->total_requests 	* 100;
    l1_inst->transfers 	= 	l1_inst->num_misses + l1_inst->flush_kickouts;

    //Calculate the l1_data things
    l1_data->total_requests = l1_data->num_hits + l1_data->num_misses;
    l1_data->hit_rate 	=	(double) l1_data->num_hits 		/ 	l1_data->total_requests 	* 100;
    l1_data->miss_rate 	= 	(double) l1_data->num_misses 	/ 	l1_data->total_requests 	* 100;
    l1_data->transfers 	= 	l1_data->num_misses + l1_data->flush_kickouts;

    //Calculate the l2 things
    l2->total_requests = l2->num_hits + l2->num_misses;
    l2->hit_rate 		=	(double) l2->num_hits 			/ 	l2->total_requests 		* 100;
    l2->miss_rate 		= 	(double) l2->num_misses 		/ 	l2->total_requests 		* 100;
    l2->transfers 		= 	l2->num_misses + l2->flush_kickouts;

    uint ICache_cost 	= (l1_inst->cache_size / 4096) 	* 100 	+ (l1_inst->cache_size / 4096) * (uint)(log(l1_inst->assoc)/log(2)) * 100;
    uint DCache_cost 	= (l1_data->cache_size / 4096) 	* 100 	+ (l1_data->cache_size / 4096) * (uint)(log(l1_data->assoc)/log(2)) * 100;
    uint L2_cache_cost 	= (l2->cache_size / 32768) 		* 25 	+ (l2->cache_size / 32768) 	   * (uint)(log(l2->assoc)/log(2))		* 25;
    int main_mem_latency_factor = (log(main_mem->mem_chunksize) / log(2)) - 4;
    uint memory_cost 	= 50 + 25 + (main_mem_latency_factor <= 0 ? 0 : main_mem_latency_factor) * 100;

    ulli exec_time = cache_results->inst_time + cache_results->read_time + cache_results->write_time +cache_results->flush_time;

    //Calculate percent of reference types
    ull total_traces 		= cache_results->num_inst + cache_results->num_reads + cache_results->num_writes;
    double inst_percent 	= ((double)cache_results->num_inst 	    / (double)total_traces) * 100;
    double read_precent 	= ((double)cache_results->num_reads 	/ (double)total_traces)	* 100;
    double write_percent 	= ((double)cache_results->num_writes 	/ (double)total_traces) * 100;

	fprintf(outputFile, "Memory System: \n");
	fprintf(outputFile, "	Dcache size = %u  :  ways = %u  :  block size = %u \n", l1_data->cache_size, l1_data->assoc, l1_data->block_size);
	fprintf(outputFile, "	Icache size = %u  :  ways = %u  :  block size = %u \n", l1_inst->cache_size, l1_inst->assoc, l1_inst->block_size);
	fprintf(outputFile, "	L2-cache size = %u  :  ways = %u  :  block size = %u \n", l2->cache_size, l2->assoc, l2->block_size);
	fprintf(outputFile, "	Memory ready time = %u  :  chunksize = %u  :  chunktime = %u \n", main_mem->mem_ready, main_mem->mem_chunksize, main_mem->mem_chunktime);
	fprintf(outputFile, "\n");

	fprintf(outputFile, "Execute time 	=	%llu;		Total refs	= %llu\n", exec_time, total_traces);
	fprintf(outputFile, "Flush time 	=	%llu\n", cache_results->flush_time);
	fprintf(outputFile, "Inst refs 		=	%llu; 	Data refs 	= %llu\n", cache_results->num_inst , cache_results->num_reads + cache_results->num_writes );
	fprintf(outputFile, "\n");

	fprintf(outputFile, "Number of reference types : 	[Percentage]\n");
	fprintf(outputFile, "	Reads 	= 	%10llu 	    [%4.1f%%]\n", cache_results->num_reads, 	read_precent);
	fprintf(outputFile, "	Writes 	= 	%10llu 	    [%4.1f%%]\n", cache_results->num_writes,	write_percent);
	fprintf(outputFile, "	Inst. 	= 	%10llu	    [%4.1f%%]\n", cache_results->num_inst, 		inst_percent);
	fprintf(outputFile, "	Total 	= 	%10llu\n ", total_traces);
	fprintf(outputFile, "\n");

	fprintf(outputFile, "Total cycles for activities: 	[Percentage]\n");
	fprintf(outputFile, "	Reads 	= 	%10llu 	    [%4.1f%%]\n", cache_results->read_time, ((double)cache_results->read_time / (double)(exec_time - cache_results->flush_time) * 100));
	fprintf(outputFile, "	Writes 	= 	%10llu 	    [%4.1f%%]\n", cache_results->write_time, ((double)cache_results->write_time / (double)(exec_time - cache_results->flush_time) * 100));
	fprintf(outputFile, "	Inst. 	= 	%10llu 	    [%4.1f%%]\n", cache_results->inst_time, ((double)cache_results->inst_time / (double)(exec_time - cache_results->flush_time) * 100));
	fprintf(outputFile, "	Total 	= 	%10llu\n ", cache_results->read_time + cache_results->write_time + cache_results->inst_time);
	fprintf(outputFile, "\n");

	fprintf(outputFile, "Average cycles per activity:\n");
	// fprintf(outputFile, "	Read = %d; Write = %d; Inst. = %d\n", , , );
	fprintf(outputFile, "Ideal: Exec. Time = %llu; CPI = %.1f\n", total_traces + cache_results->num_inst, round(10*(((double)total_traces + (double)cache_results->num_inst)/ (double)cache_results->num_inst))/10);
	fprintf(outputFile, "Ideal mis-aligned: Exec. Time = %llu; CPI = %.1f\n", l1_inst->total_requests + l1_data->total_requests + cache_results->num_inst, round(10*((double)l1_inst->total_requests + (double)l1_data->total_requests + (double)cache_results->num_inst)/ (double)cache_results->num_inst)/10);
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

	fprintf(outputFile, "L1 cache cost (Icache $%d) + (Dcache $%d) = $%d\n", ICache_cost, DCache_cost, ICache_cost + DCache_cost);
	fprintf(outputFile, "L2 cache cost = $%d; Memory cost = $%d; Total cost = $%d\n", L2_cache_cost, memory_cost, ICache_cost + DCache_cost + L2_cache_cost + memory_cost);
	fprintf(outputFile, "Flushes = %llu  :  Invalidates = %llu\n", cache_results->flush_cnt, cache_results->num_invalid);
	fprintf(outputFile, "\n");

	fprintf(outputFile, "Memory Level:  L1i\n");

	print_cache(l1_inst, outputFile);
	
	fprintf(outputFile, "Memory Level:  L1d\n");

	print_cache(l1_data, outputFile);
	
	fprintf(outputFile, "Memory Level:  L2\n");

	print_cache(l2, outputFile);

	

	fclose(outputFile);
}

void print_cache(cache* cache_level, FILE * outputFile){
	for(ulli i = 0; i < cache_level->num_sets; i++){
		if(cache_level->cache_set[i].block[0].valid == true){
			fprintf(outputFile, "Index: %4llx | V:1 D:%d Tag: %12llx | ", i, cache_level->cache_set[i].block[0].dirty, cache_level->cache_set[i].block[0].tag);
			for(ulli j = 1; j < cache_level->assoc; j++){
				if(cache_level->cache_set[i].block[j].valid == true){
					fprintf(outputFile,  "V:%d D:%d Tag: %12llx | ", cache_level->cache_set[i].block[j].valid, cache_level->cache_set[i].block[j].dirty, cache_level->cache_set[i].block[j].tag);
				}
				else if(cache_level->cache_set[i].block[j].valid == false){
					fprintf(outputFile,  "V:%d D:%d Tag: %12c | ", cache_level->cache_set[i].block[j].valid, cache_level->cache_set[i].block[j].dirty, '-');
				}
			}
			fprintf(outputFile, "\n");
		}	
	}
	fprintf(outputFile, "\n");
}