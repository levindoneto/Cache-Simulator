/**************************** Libraries ***************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
/******************************************************************************/

/***************************** Structs ****************************************/
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
    int    ** Cache_Data;  // The blocks are Cache_Data[set][line], with all words
                           //    from the block.
                           // Cache_Data [set][line], that's enough, because in
                           //    case we don't need informations about word and
                           //    all words have one byte, and the fourth position of
                           //    the array would be for acess the bytes in a word
                           //    Cache [number_of_sets][cache_description.associativity].
                           // This data is modeled with a value 1 (there is data)
                           //    and  0 (there isn't data) in the block.

    long unsigned    ** Cache_Upper; // The informations about Upper is used to differentiate
                           //    two or more blocks with the same set (index)
                           //    information. Sets aren't ordered by upper or any
                           //    particular order.

    //int    ** Dirty_Bit;  // That is used in the write back polocy for blocks
                          //     modified only in the cache memory.
                          //     DIRTY: 1, CLEAN: 0.

    time_t ** T_Access;   // Time stamp for the access in the cache memory (used
                          //     in the LRU Algorithm).

    time_t ** T_Load;     // Time stamp for the data load in the cache memory
                          //     (used in the FIFO Algorithm).
} Cache;

/******************************************************************************/

/************************ Prototypes of used functions ************************/
int make_upper(long unsigned address, int words_per_line, int bytes_per_word);
int make_index(int number_of_lines_of_cache, long unsigned upper);
int make_tag(int number_of_lines_of_cache, int associativity);
int getPosUpper (Cache cache, int index, long unsigned line, int associativity);
void write_cache (Cache cache1, int index1, long unsigned line1, int data1, int associativity);
int read_cache (Cache cache1, int index1, long unsigned line1, int data1, int associativity);
void generate_output(Results cache_results);
/******************************************************************************/
