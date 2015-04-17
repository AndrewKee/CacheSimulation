#ifndef CACHE_H
#define CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

typedef unsigned int uint;
// typedef enum { false, true } bool;
typedef unsigned long long ull;
typedef unsigned long long int ulli;

typedef struct results 
{
	ulli num_inst;
	ulli num_reads;
	ulli num_writes;

	ulli flush_time;
	ulli write_time;
	ulli read_time;
	ulli inst_time;

	ulli flush_cnt;
	ulli num_invalid;
} results;

typedef struct cache {
	//cache parameters
	uint block_size;
	uint cache_size;
	uint assoc;
	uint hit_time;
	uint miss_time;
	uint transfer_time;
	uint bus_width;

	uint num_sets;
	uint tag_size;

	//cache set points to array of sets
	struct cache_set* cache_set;

	//must know where to go next if we get a miss
	struct cache* next_level;

	//main memory parameters
	uint mem_sendaddr;
	uint mem_ready;
	uint mem_chunktime;
	uint mem_chunksize;

	//keep track of hits and misses
	ull num_hits;
	ull num_misses;
	ull total_requests;

	double hit_rate;
	double miss_rate;

	ull kickouts;
	ull dirty_kickouts;
	ull transfers;
	ull flush_kickouts;
} cache;

//Cache set structure
typedef struct cache_set {
	struct cache_block* block;
	struct LRU* lru;
} cache_set;

typedef struct cache_block {
	ulli tag;
	bool valid;
	bool dirty;
	ulli address;
} cache_block;


//parse through the config file.
//Will store the values into the cache structs l1_data, l1_inst, l2, and main_mem
//these are all just properties of each of the caches and the main memory 
int parse_config(char* filename, cache* l1_data, cache* l1_inst, cache* l2, cache* main_mem);

void allocate_blocks(cache* l1_data, cache* l1_inst, cache* l2);

//loops through the traces and does the trace
void read_trace(cache* l1_data, cache* l1_inst, cache* l2, results* cache_results);

uint search_cache(cache* cache_level, ulli address, char type);

void look_through_cache(cache* cache_level, ulli address, char type, ulli num_bytes, ulli index);

ulli get_tag(cache* cache_level, ulli address);

ulli get_index(cache* cache_level, ulli address);

ulli get_byte_offset(cache* cache_level, ulli address);

ulli create_address(cache* cache_level, ulli tag, ulli index, ulli byte_offset);

ulli prep_search_cache(cache* cache_level, ulli address, uint bytesize, char op);

int num_indices(cache* cache_level, ulli address, uint bytesize);

//outputs the results into a file
void report(cache* l1_data, cache* l1_inst, cache* l2, cache* main_mem, results* cache_results);

void print_cache(cache* cache_level, FILE * outputFile);

//Flushes the cache
uint flush(cache* cache_level);

//Transfer up or down a level
uint transfer(cache* cache_level);

void init_cache(cache* cache_level);

//Allocate a cache's contents
void cache_alloc(cache* cache_level);

#endif
