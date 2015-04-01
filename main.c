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
	parse_config("config.dat", &l1_data, &l1_inst, &l2, &main_mem);
	char op;
	ull address;
	uint bytesize;
	while(scanf("%c %llu %d\n", &op, &address, &bytesize) == 3){
		printf("in the trace\n");
		if(op == 'I'){
			num_inst++;
		} else if (op == 'R'){
			num_reads++;
		} else if (op == 'W'){
			num_writes++;
		}
	}
	printf("%llu \n", num_inst);
	// read_trace(&num_inst, &num_reads, &num_writes);//pass tracefile name, num_inst, num_reads, num_writes
	//report(&l1_data, &l1_inst, &l2, &main_mem, &num_inst, &num_reads, &num_writes);
}



