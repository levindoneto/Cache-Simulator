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
    int line_size;                                   // Block size
    int number_of_lines;                             // Number of blocks
    int associativity;                               // Number of blocks per set
    char replacement_policy[5];                      // LRY or FIFO + \0
} Desc;

typedef struct cache {
    int *** Cache_Data;  // The blocks are Cache_Data[set][line], with all words
                         //    from the block.
                         //  Cache_Data [set][line][word], that's enough, because all words
                         //    in this case have one byte, and the fourth position of
                         //    the array would be for acess the bytes in a word
                         //    Cache [number_of_sets][cache_description.number_of_lines][words_per_line]
    float *** T_Access; // Time stamp for the access in the cache memory (used
                        //     in the LRU Algorithm)
    float *** T_Load;   // Time stamp for the data load in the cache memory
                        //     (used in the FIFO Algorithm)
} Cache;

// Prototypes of used functions
void generate_output(Results cache_results);
int make_upper(int address, int words_per_line, int bytes_per_word);
int make_index(int number_of_lines_of_cache, int associativity);
int make_tag(int number_of_lines_of_cache, int associativity);
