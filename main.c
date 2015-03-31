/*
	Cache Simulation Project -- ECEN 4593
	Authors: 
		Andrew Kee
		Ryan Montoya
*/

#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include "cache.h"

int main(int argc, char *argv[]){
	struct cache l1_data, l1_inst, l2, main_mem;
	parse_config("config.dat", &l1_data, &l1_inst, &l2, &main_mem);
}



