/*
	Cache Simulation Project -- ECEN 4593
	Authors: 
		Andrew Kee
		Ryan Montoya
*/

#include <stdio.h>
#include <stdlib.h>
#include "cache.h"

int main(int argc, char *argv[]){
	cache* l1_data  = malloc( sizeof(cache));
	cache* l1_inst  = malloc( sizeof(cache));
	cache* l2 		= malloc( sizeof(cache));
	cache* main_mem = malloc( sizeof(cache));

	init_cache(l1_data);
	init_cache(l1_inst);
	init_cache(l2);


	results* cache_results = malloc( sizeof(results));
	cache_results->num_inst = 0;
	cache_results->num_reads = 0;
	cache_results->num_writes = 0;
	cache_results->flush_time = 0;
	cache_results->read_time = 0;
	cache_results->inst_time = 0;
	cache_results->flush_cnt = 0;
	cache_results->num_invalid = 0;

	char * x;
	if (argv[1]) 
		x = argv[1];
	else 
		x = "Config/defaults.dat";

	printf("%s\n", x);
	parse_config(x, l1_data, l1_inst, l2, main_mem);
	allocate_blocks(l1_data, l1_inst, l2);
	read_trace(l1_data, l1_inst, l2, cache_results);
	report(l1_data, l1_inst, l2, main_mem, cache_results);

	return 0;
}