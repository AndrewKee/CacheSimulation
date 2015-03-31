#include "cache.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void parse_config(char* filename, cache l1_data, cache l1_inst, cache l2, cache main_mem){
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
					l1_data.block_size = val;
					l1_inst.block_size = val;
				} else if (strcmp(input, "cache_size") == 0){
					l1_data.cache_size = val;
					l1_inst.cache_size = val;
				} else if (strcmp(input, "assoc") == 0){
					l1_data.assoc = val;
					l1_inst.assoc = val;
				} else if (strcmp(input, "hit_time") == 0){
					l1_data.hit_time = val;
					l1_inst.hit_time = val;
				} else if (strcmp(input, "miss_time") == 0){
					l1_data.miss_time = val;
					l1_inst.miss_time = val;
				}				
			} else if (strcmp(input, "L2") == 0){
				if (strcmp(input, "block_size") == 0){
					l2.block_size = val;
				} else if (strcmp(input, "cache_size") == 0){
					l2.cache_size = val;
				} else if (strcmp(input, "assoc") == 0){
					l2.assoc = val;
				} else if (strcmp(input, "hit_time") == 0){
					l2.hit_time = val;
				} else if (strcmp(input, "miss_time") == 0){
					l2.miss_time = val;
				} else if (strcmp(input, "transfer_time") == 0){
					l2.transfer_time = val;
				} else if (strcmp(input, "bus_width") == 0){
					l2.bus_width = val;
				}
			} else if (strcmp(input, "mm") == 0){
				if (strcmp(input, "mem_sendaddr") == 0){
					main_mem.mem_sendaddr = val;
				} else if (strcmp(input, "mem_ready") == 0){
					main_mem.mem_ready = val;
				} else if (strcmp(input, "mem_chunktime") == 0){
					main_mem.mem_chunktime = val;
				} else if (strcmp(input, "mem_chunksize") == 0){
					main_mem.mem_chunksize = val;
				}
			}
			// printf("%s %s %d\n", cacheLevel, input, val);
		}
	}
}

void report(){

}