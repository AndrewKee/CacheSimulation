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

static struct cache l1_data, l1_inst, l2, main_mem;

int main(int argc, char *argv[]){
	parse_config("config.dat", l1_data, l1_inst, l2, main_mem);
}



