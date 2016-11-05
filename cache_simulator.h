// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct results {
    int acess_count;
    int read_hits;
    int read_misses;
    int write_hits;
    int write_misses;
} Results;

typedef struct desc {
    int line_size;               // Block size
    int number_of_lines;         // Number of blocks
    int associativity;           // Number of blocks per set
    char replacement_policy[5];  // LRY or FIFO + \0
} Desc;

// Prototype of used functions
void generate_output(Results cache_results);
int make_line(int address, int words_per_line, int bytes_per_word);
int make_index(int number_of_lines_of_cache, int associativity);
int make_tag(int number_of_lines_of_cache, int associativity);
