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
	struct cache l1_data, l1_inst, l2, main_mem;
	ull num_inst 	= 0;
	ull num_reads	= 0;
	ull num_writes 	= 0;

	char * x;
	if (argv[1]) 
		x = argv[1];
	else 
		x = "Config/defaults.dat";

	printf("%s\n", x);
	parse_config(x, &l1_data, &l1_inst, &l2, &main_mem);
	allocate_blocks(&l1_data, &l1_inst, &l2);
	read_trace(&l1_data, &l1_inst, &num_inst, &num_reads, &num_writes);
	report(&l1_data, &l1_inst, &l2, &main_mem, &num_inst, &num_reads, &num_writes);

	return 0;
}



