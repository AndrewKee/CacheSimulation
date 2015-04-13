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

	ull* num_inst  = malloc( sizeof(ull));
	ull* num_reads = malloc( sizeof(ull));
	ull* num_writes= malloc( sizeof(ull));

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
<<<<<<< HEAD
	allocate_blocks(l1_data, l1_inst, l2, main_mem);
	read_trace(l1_data, l1_inst, num_inst, num_reads, num_writes);
=======
	allocate_blocks(l1_data, l1_inst, l2);
	read_trace(l1_data, l1_inst, l2, num_inst, num_reads, num_writes);
>>>>>>> 5884a3024707872eb2f287f0519eb18f6b776cc3
	report(l1_data, l1_inst, l2, main_mem, num_inst, num_reads, num_writes);

	return 0;
}