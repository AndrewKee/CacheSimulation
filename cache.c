#include "cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_config(char* filename, struct cache* l1_data, struct cache* l1_inst, struct cache* l2, struct cache* main_mem){
	FILE *fp;
	char input[20];
	char cacheLevel[10];
	int val;
	// printf("%s \n", filename);
	fp = fopen(filename, "r");

	if(fp == NULL){
		printf("File Don't Exist \n");
	}
	else{  
		while (fscanf(fp,"%s %s %d\n",cacheLevel, input, &val) == 3){
			if(strcmp(cacheLevel, "L1") == 0){
				if (strcmp(input, "block_size") == 0){
					l1_data->block_size = val;
					l1_inst->block_size = val;
				} else if (strcmp(input, "cache_size") == 0){
					l1_data->cache_size = val;
					l1_inst->cache_size = val;
				} else if (strcmp(input, "assoc") == 0){
					l1_data->assoc = val;
					l1_inst->assoc = val;
				} else if (strcmp(input, "hit_time") == 0){
					l1_data->hit_time = val;
					l1_inst->hit_time = val;
				} else if (strcmp(input, "miss_time") == 0){
					l1_data->miss_time = val;
					l1_inst->miss_time = val;
				}				
			} else if (strcmp(input, "L2") == 0){
				if (strcmp(input, "block_size") == 0){
					l2->block_size = val;
				} else if (strcmp(input, "cache_size") == 0){
					l2->cache_size = val;
				} else if (strcmp(input, "assoc") == 0){
					l2->assoc = val;
				} else if (strcmp(input, "hit_time") == 0){
					l2->hit_time = val;
				} else if (strcmp(input, "miss_time") == 0){
					l2->miss_time = val;
				} else if (strcmp(input, "transfer_time") == 0){
					l2->transfer_time = val;
				} else if (strcmp(input, "bus_width") == 0){
					l2->bus_width = val;
				}
			} else if (strcmp(input, "mm") == 0){
				if (strcmp(input, "mem_sendaddr") == 0){
					main_mem->mem_sendaddr = val;
				} else if (strcmp(input, "mem_ready") == 0){
					main_mem->mem_ready = val;
				} else if (strcmp(input, "mem_chunktime") == 0){
					main_mem->mem_chunktime = val;
				} else if (strcmp(input, "mem_chunksize") == 0){
					main_mem->mem_chunksize = val;
				}
			}
		}
	}
}

void read_trace(){
	
}

void report(struct cache* l1_data, struct cache* l1_inst, struct cache* l2, struct cache* main_mem, ull* num_inst, ull* num_reads, ull* num_writes){

	//Calculate the l1_inst things
	l1_inst->total_req = l1_inst->num_hits + l1_inst->num_misses;
    l1_inst->hit_rate 	=	(double) l1_inst->num_hits 		/ 	l1_inst->total_req 	* 100;
    l1_inst->miss_rate 	= 	(double) l1_inst->num_misses 	/ 	l1_inst->total_req 	* 100;

    //Calculate the l1_data things
    l1_data->total_req = l1_data->num_hits + l1_data->num_misses;
    l1_data->hit_rate 	=	(double) l1_data->num_hits 		/ 	l1_data->total_req 	* 100;
    l1_data->miss_rate 	= 	(double) l1_data->num_misses 	/ 	l1_data->total_req 	* 100;

    //Calculate the l2 things
    l2->total_req = l2->num_hits + l2->num_misses;
    l2->hit_rate 		=	(double) l2->num_hits 			/ 	l2->total_req 		* 100;
    l2->miss_rate 		= 	(double) l2->num_misses 		/ 	l2->total_req 		* 100;

    //Calculate percent of reference types
    ull total_traces 		= num_inst + num_reads + num_writes;
    double inst_percent 	= num_inst 		/ total_traces 	* 100;
    double read_precent 	= num_reads 	/ total_traces 	* 100;
    double write_percent 	= num_writes 	/ total_traces 	* 100;

	printf("Memory System: \n");
	printf("	Dcache size = %d  :  ways = %d  :  block size = %d \n", l1_data->cache_size, , l1_data->block_size);
	printf("	Icache size = %d  :  ways = %d  :  block size = %d \n", l1_inst->cache_size, , l1_inst->block_size);
	printf("	L2-cache size = %d  :  ways = %d  :  block size = %d \n", l2->cache_size, , l2->block_size);
	printf("	Memory ready time = %d  :  chunksize = %d  :  chunktime = %d \n", main_mem->mem_ready, main_mem->chunksize, main_mem->chunktime);
	printf("\n");

	printf("Execute time 	=	%f;		Total refs	= %f\n", , l1_inst->total_req + l1_data->total_req);
	printf("Flush time 		=	%f", );
	printf("Inst refs 		=	%f; 	Data refs 	= %f", l1_inst->total_req, l1_data->total_req);
	printf("\n");

	printf("Number of reference types : 	[Percentage]\n");
	printf("	Reads 	= 	%llu 			[%d]\n", num_reads, 	read_precent);
	printf("	Writes 	= 	%llu 			[%d]\n", num_writes, 	write_percent);
	printf("	Inst. 	= 	%llu			[%d]\n", num_inst, 		inst_percent);
	printf("	Total 	= 	%llu\n ", );
	printf("\n");

	printf("Total cycles for activities: 	[Percentage]\n");
	printf("	Reads 	= 	%llu 			[%d]\n", , );
	printf("	Writes 	= 	%llu 			[%d]\n", , );
	printf("	Inst. 	= 	%llu 			[%d]\n", , );
	printf("	Total 	= 	%llu\n ", );
	printf("\n");

	printf("Average cycles per activity:\n");
	printf("	Read = %d; Write = %d; Inst. = %d\n", , , );
	printf("Ideal: Exec. Time = %f; CPI = %d\n", , );
	printf("Ideal mis-aligned: Exec. Time = %f; CPI = %d\n", , );
	printf("\n");

	printf("Memory Level: 	L1i");
	printf("	Hit Count = %llu 		Miss Count = %llu\n", l1_inst->num_hits, l1_inst->num_misses);
	printf("	Total Requests = %llu", l1_inst->total_req);
	printf(" 	Hit Rate = %d%% 	Miss Rate = %d%%\n", l1_inst->hit_rate, l1_inst->miss_rate);
	printf("	Kickouts = %f; Dirty Kickouts = %f; Transfers = %f\n", , , );
	printf("Flush Kickouts = %f", );
	printf("\n");

	printf("Memory Level: 	L1d");
	printf("	Hit Count = %f 		Miss Count = %f\n", l1_data->num_hits, l1_data->num_misses);
	printf("	Total Requests = %f", l1_data->total_req);
	printf(" 	Hit Rate = %d%% 	Miss Rate = %d%%\n", l1_data->hit_rate, l1_data->miss_rate);
	printf("	Kickouts = %f; Dirty Kickouts = %f; Transfers = %f\n", , , );
	printf("Flush Kickouts = %f", );
	printf("\n");

	printf("Memory Level: 	L2");
	printf("	Hit Count = %f 		Miss Count = %f\n", l2->num_hits, l2->num_misses);
	printf("	Total Requests = %f", l2->total_req);
	printf(" 	Hit Rate = %d%% 	Miss Rate = %d%%\n", l2->hit_rate, l2->miss_rate);
	printf("	Kickouts = %f; Dirty Kickouts = %f; Transfers = %f\n", , , );
	printf("Flush Kickouts = %f", );
	printf("\n");

	printf("L1 cache cost (Icache $%d) + (Dcache $%d) = $%d", , , );
	printf("L1 cache cost = $%d; Memory cost = $%d; Total cost = $%d", , , );
	printf("Flushes = %f  :  Invalidates = %f", , );
}






