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
	// l1_data 	= malloc( sizeof(cache));
	// l1_inst 	= malloc( sizeof(cache));
	// l2 			= malloc( sizeof(cache));
	// main_mem 	= malloc( sizeof(cache));
	ulli* num_inst  = malloc( sizeof(ulli));
	ulli* num_reads = malloc( sizeof(ulli));
	ulli* num_writes= malloc( sizeof(ulli));

	*num_inst  	= 0;
	*num_reads 	= 0;
	*num_writes = 0;

	char * x;
	if (argv[1]) 
		x = argv[1];
	else 
		x = "Config/defaults.dat";

	printf("%s\n", x);
	parse_config(x, l1_data, l1_inst, l2, main_mem);
	allocate_blocks(l1_data, l1_inst, l2);
	read_trace(l1_data, l1_inst, l2, num_inst, num_reads, num_writes);
	report(l1_data, l1_inst, l2, main_mem, num_inst, num_reads, num_writes);

	return 0;
}