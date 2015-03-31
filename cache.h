typedef struct cache {
	//cache parameters
	unsigned int block_size;
	unsigned int cache_size;
	unsigned int assoc;
	unsigned int hit_time;
	unsigned int miss_time;
	unsigned int transfer_time;
	unsigned int bus_width;

	//main memory parameters
	unsigned int mem_sendaddr;
	unsigned int mem_ready;
	unsigned int mem_chunktime;
	unsigned int mem_chunksize;
} cache;

void parse_config(char* filename, struct cache* l1_data, struct cache* l1_inst, struct cache* l2, struct cache* main_mem);

void report();