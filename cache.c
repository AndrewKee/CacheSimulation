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
		//read through the config file, 
		//the config file is formatted so that we have 
			// cacheLevel(string) property(string) value(integer)
		// repeated over new lines
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

	FILE * outputFile;

	outputFile = fopen("results.dat", "w+");

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

	fprintf(outputFile, "Memory System: \n");
	fprintf(outputFile, "	Dcache size = %u  :  ways = %u  :  block size = %u \n", l1_data->cache_size, , l1_data->block_size);
	fprintf(outputFile, "	Icache size = %u  :  ways = %u  :  block size = %u \n", l1_inst->cache_size, , l1_inst->block_size);
	fprintf(outputFile, "	L2-cache size = %u  :  ways = %u  :  block size = %u \n", l2->cache_size, , l2->block_size);
	fprintf(outputFile, "	Memory ready time = %u  :  chunksize = %u  :  chunktime = %u \n", main_mem->mem_ready, main_mem->chunksize, main_mem->chunktime);
	fprintf(outputFile, "\n");

	fprintf(outputFile, "Execute time 	=	%llu;		Total refs	= %llu\n", , l1_inst->total_req + l1_data->total_req);
	fprintf(outputFile, "Flush time 		=	%llu", );
	fprintf(outputFile, "Inst refs 		=	%llu; 	Data refs 	= %llu", l1_inst->total_req, l1_data->total_req);
	fprintf(outputFile, "\n");

	fprintf(outputFile, "Number of reference types : 	[Percentage]\n");
	fprintf(outputFile, "	Reads 	= 	%llu 			[%d]\n", num_reads, 	read_precent);
	fprintf(outputFile, "	Writes 	= 	%llu 			[%d]\n", num_writes, 	write_percent);
	fprintf(outputFile, "	Inst. 	= 	%llu			[%d]\n", num_inst, 		inst_percent);
	fprintf(outputFile, "	Total 	= 	%llu\n ", total_traces);
	fprintf(outputFile, "\n");

	fprintf(outputFile, "Total cycles for activities: 	[Percentage]\n");
	fprintf(outputFile, "	Reads 	= 	%llu 			[%d]\n", , );
	fprintf(outputFile, "	Writes 	= 	%llu 			[%d]\n", , );
	fprintf(outputFile, "	Inst. 	= 	%llu 			[%d]\n", , );
	fprintf(outputFile, "	Total 	= 	%llu\n ", );
	fprintf(outputFile, "\n");

	fprintf(outputFile, "Average cycles per activity:\n");
	fprintf(outputFile, "	Read = %d; Write = %d; Inst. = %d\n", , , );
	fprintf(outputFile, "Ideal: Exec. Time = %llu; CPI = %d\n", , );
	fprintf(outputFile, "Ideal mis-aligned: Exec. Time = %llu; CPI = %d\n", , );
	fprintf(outputFile, "\n");

	fprintf(outputFile, "Memory Level: 	L1i");
	fprintf(outputFile, "	Hit Count = %llu 		Miss Count = %llu\n", l1_inst->num_hits, l1_inst->num_misses);
	fprintf(outputFile, "	Total Requests = %llu", l1_inst->total_req);
	fprintf(outputFile, " 	Hit Rate = %d%% 	Miss Rate = %d%%\n", l1_inst->hit_rate, l1_inst->miss_rate);
	fprintf(outputFile, "	Kickouts = %llu; Dirty Kickouts = %llu; Transfers = %llu\n", l1_inst->Kickouts, l1_inst->dirty_kickouts, l1_inst->transfers);
	fprintf(outputFile, "Flush Kickouts = %llu", l1_inst->flush_kickouts);
	fprintf(outputFile, "\n");

	fprintf(outputFile, "Memory Level: 	L1d");
	fprintf(outputFile, "	Hit Count = %llu 		Miss Count = %llu\n", l1_data->num_hits, l1_data->num_misses);
	fprintf(outputFile, "	Total Requests = %llu", l1_data->total_req);
	fprintf(outputFile, " 	Hit Rate = %d%% 	Miss Rate = %d%%\n", l1_data->hit_rate, l1_data->miss_rate);
	fprintf(outputFile, "	Kickouts = %llu; Dirty Kickouts = %llu; Transfers = %llu\n", l1_data->Kickouts, l1_data->dirty_kickouts, l1_data->transfers);
	fprintf(outputFile, "Flush Kickouts = %llu", l1_data->flush_kickouts);
	fprintf(outputFile, "\n");

	fprintf(outputFile, "Memory Level: 	L2");
	fprintf(outputFile, "	Hit Count = %llu 		Miss Count = %llu\n", l2->num_hits, l2->num_misses);
	fprintf(outputFile, "	Total Requests = %llu", l2->total_req);
	fprintf(outputFile, " 	Hit Rate = %d%% 	Miss Rate = %d%%\n", l2->hit_rate, l2->miss_rate);
	fprintf(outputFile, "	Kickouts = %llu; Dirty Kickouts = %llu; Transfers = %llu\n", l2->Kickouts, l2->dirty_kickouts, l2->transfers);
	fprintf(outputFile, "Flush Kickouts = %llu", l2->flush_kickouts);
	fprintf(outputFile, "\n");

	fprintf(outputFile, "L1 cache cost (Icache $%d) + (Dcache $%d) = $%d", , , );
	fprintf(outputFile, "L1 cache cost = $%d; Memory cost = $%d; Total cost = $%d", , , );
	fprintf(outputFile, "Flushes = %f  :  Invalidates = %f", , );

	fclose(outputFile);
}






