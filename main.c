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
	struct cache l1_data, l1_inst, l2;
	struct main_memory main_mem;
	ull num_inst 	= 0;
	ull num_reads	= 0;
	ull num_writes 	= 0;
	printf("%s\n", argv[1]);
	parse_config(argv[1], &l1_data, &l1_inst, &l2, &main_mem);

	read_trace(&num_inst, &num_reads, &num_writes);//pass tracefile name, num_inst, num_reads, num_writes
	report(&l1_data, &l1_inst, &l2, &main_mem, &num_inst, &num_reads, &num_writes);
}



