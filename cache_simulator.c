/**
  * --------------------------------------------------------------------------
  * Cache Simulator
  * Levindo Neto (https://github.com/levindoneto/Cache-Simulator)
  * --------------------------------------------------------------------------
  */
#include <unistd.h>
// Libraries
#include <stdio.h>
#include "cache_simulator.h"           // Header file with the structs and functions prototypes
#include <stdlib.h>
#include <string.h>
#include <time.h>                      // For the time stamp stuff
#define DEBUG 0                        // 0: FALSE, 1:TRUE
#define CACHEVIEW 0                        // 1: print the currently cache (data and upper), 0: do not print
#define BYTES_PER_WORD 1               // All words with 1 Byte (bytes_per_word)
#define DATA 1


int currently_clk=0;                   // To simulate a clock with a counter

/**************************** Functions ***************************************/

/**
 * This function makes the start of all cache with zeros.
 */
int startCache(Cache *cache1, int number_of_sets, int associativity) {
      int index_i, block_i;
      for (index_i=0; index_i<number_of_sets; index_i++) {
          for (block_i=0; block_i<associativity; block_i++ ) {
              cache1->Cache_Data[index_i][block_i]  = 0;
              cache1->Cache_Upper[index_i][block_i] = 0;
              cache1->T_Access[index_i][block_i]    = 0;
              cache1->T_Load[index_i][block_i]      = 0;
          }
      }
      return 0;
}

/**
 * This function get the line (upper) of a given address by the CPU
 * This upper is equal to the Tag information
 */
int make_upper(long unsigned address, int words_per_line, int bytes_per_word) {
    int line;
    line = ((address/words_per_line))/bytes_per_word;
    return line;
}

/**
 * This function generates the index for the set in the cache
 */
int make_index (int number_of_sets, long unsigned upper) {
    int index;
    //printf("NUMBER OF SETS: %d\n", number_of_sets);
    //printf("UPPER: %lu\n", upper);
    index = upper % number_of_sets;

    return index;                               // index of the set in the cache
}

/**
 * This function give the position in a set (by index) of a line with a determined upper
 */
int getPosUpper (Cache *cache, int index, long unsigned line, int associativity) {
    int block_i;
    //printf("INDEX POS UPPER %d, ASSOC: %d, LINE: %lu\n", index, associativity, line);
    for (block_i=0; block_i<associativity; block_i++) {
        if (cache->Cache_Upper[index][block_i] == line){
            return block_i;               // Upper inside of the set found
        }
    }
    return -1;                           // Upper inside of the set not found
}

/** This function verifies if are or aren't free blocks (lines) available in
  *     a set (by index1) in the cache memory.
  * @return: 1 (set is full), -1 (There are free space in the set by index1)
  */
int there_Are_Space_Set(Cache *cache1, int index1, int associativity) {
    int block_i;
    for (block_i=0; block_i<associativity; block_i++) {
        if (cache1->Cache_Data[index1][block_i]==0) {
            return -1;
        }
    }
    return 1;
}

/** This function gives a block (line) free in a set (by index1) of the cache
  *     memory with no particular order. It returns the first block (position of line)
  *      free found, independent of its positon at the set.
  */
int random_free_space_set (Cache *cache1, int index1, int associativity) {
    int block_i;
    for (block_i=0; block_i<associativity; block_i++) {
        if (cache1->Cache_Data[index1][block_i]==0) {
            return block_i;
        }
    }
    return -1;                                   // None block free to be filled
}

int findLessAccessTSset (Cache *cache1, int index1, int associativity) {
    int block_i;
    int p_lessAc=0; // Position when is the less T_Access in the set (by index)
    long unsigned lessAc = cache1->T_Access[index1][0];
    for (block_i=0; block_i<associativity; block_i++) {
        if (cache1->T_Access[index1][block_i] < lessAc) {
            lessAc = cache1->T_Access[index1][block_i];
            p_lessAc = block_i;
        }
    }
    return p_lessAc;
}

int findLessLoadTSset (Cache *cache1, int index1, int associativity) {
    int block_i;
    int p_lessLd=0;  // Position when is the less T_Access in the set (by index)
    long unsigned lessLd = cache1->T_Load[index1][0];
    for (block_i=0; block_i<associativity; block_i++) {
        if (cache1->T_Load[index1][block_i] < lessLd) {              // b < b+1
            lessLd = cache1->T_Load[index1][block_i];
            p_lessLd = block_i;
        }
    }
    return p_lessLd;
}

void write_cache (Cache *cache1, Results *result1, int index1, long unsigned line1, int data1, int associativity, char *replacement_policy) {
    /** Writing the data in the set (by index) in the position that contains the
      *     upper (by line).
      */
    char *lru  = "LRU\0";
    char *fifo = "FIFO\0";
    int is_full;
    int rpl, rpf;
    int free_block;
    int p_lAc, p_lLd;     // Position of the less counter for LRU(l) and FIFO(f)

    int position_that_has_this_upper = getPosUpper(cache1, index1, line1, associativity);

    /****************************** Write Miss ********************************/
    if (position_that_has_this_upper == -1) { // position with the right upper was not found
        /** None position contains a line with the solicited upper.
          * The cache can be full, so it have to use a replacement policy (FIFO or LRU).
          * If it still a place in the cache, it's enough to draft one of free
          *     lines to put the new upper and the "new" data.
          * To know if are free places in the set (by index1) of cache, it is
          *     used the function there_Are_Space_Set(...).
          */
        result1->write_misses++; // (*result1).write_misses++  this points to the cache_mem in the main

        /** It is necessary to know if the set is full (it will use FIFO or LRU
          *     replacement policy) or not.
          */
        is_full = there_Are_Space_Set(cache1, index1, associativity);

        /*************************** Set is full *****************************/
        if (is_full == 1) {
            // *** LRU ***
            rpl = strcmp(lru, replacement_policy);
            if (rpl == 0) { // It's LRU
                p_lAc = findLessAccessTSset(cache1, index1, associativity);
                cache1->Cache_Upper[index1][p_lAc] = line1; // line1 = upper
                cache1->Cache_Data[index1][p_lAc] = DATA;
                cache1->T_Access[index1][p_lAc] = currently_clk;  // Update T_Access
                cache1->T_Load[index1][p_lAc]   = currently_clk ; // Update T_Load
            }
            // *** FIFO ***
            rpf = strcmp(fifo, replacement_policy);
            if (rpf == 0) { // It's FIFO
                p_lLd = findLessLoadTSset(cache1, index1, associativity);

                cache1->Cache_Upper[index1][p_lLd] = line1;
                cache1->Cache_Data[index1][p_lLd] = DATA;
                cache1->T_Access[index1][p_lLd] = currently_clk; // Update T_Access
                cache1->T_Load[index1][p_lLd] = currently_clk;   // Update T_Load
            }

        }
        /**********************************************************************/

        /************************** Set isn't full ****************************/
        else {                  // There are a free block in the set (by index1)
            free_block = random_free_space_set (cache1, index1, associativity);

            cache1->Cache_Upper[index1][free_block] = line1;
            cache1->Cache_Data[index1][free_block] = DATA;
            cache1->T_Access[index1][free_block] = currently_clk; // Update the T_Access
            cache1->T_Load[index1][free_block] = currently_clk;   // Update the T_Load
        }
    }
    /**************************************************************************/

    /****************************** Write Hit ********************************/
    else {               // position with the right upper was found
        /** The position with the upper was found in the passed set (index1)
         *     The T_Access and T_Load is updated in this case, because a number_of_writes
         *     data is writed in a position at the set in cache that already have
         *     an another data.
         */
        result1->write_hits++;
        cache1->Cache_Upper[index1][position_that_has_this_upper] = line1;
        //cache1->Cache_Data[index1][position_that_has_this_upper] = DATA; // Write the "new" data in the right position at the cache memory
        cache1->T_Access[index1][position_that_has_this_upper] = currently_clk; // Update the T_Access
    }
}

void read_cache (Cache *cache1, Results *result1, int index1, long unsigned line1, int data1, int associativity, char *replacement_policy) {
    /** Reading the data in the set (by index) in the position that contains the
      *     upper (by line).
      */
    char *lru  = "LRU\0";
    char *fifo = "FIFO\0";
    int is_full;
    int rpl, rpf;
    int free_block;
    int p_lAc, p_lLd;     // Position of the less counter for LRU(l) and FIFO(f)
    int position_that_has_this_upper = getPosUpper(cache1, index1, line1, associativity);

    /****************************** Read Misses ********************************/
    if (position_that_has_this_upper == -1) { // position with the right upper was not found
        /** None position contains a line with the solicited upper.
          * The cache can be full, so it have to use a replacement policy (FIFO or LRU).
          * If it still a place in the cache, it's enough to draft one of free
          *     lines to put the new upper and the "new" data.
          * To know if are free places in the set (by index1) of cache, it is
          *     used the function there_Are_Space_Set(...).
          */
        result1->read_misses++; // (*result1).write_misses++  this points to the cache_mem in the main

        /** It is necessary to know if the set is full (it will use FIFO or LRU
          *     replacement policy) or not.
          */
        is_full = there_Are_Space_Set(cache1, index1, associativity);

        /*************************** Set is full ******************************/
        if (is_full == 1) {
            //printf("SET FULL: %d\n", index1);
            // *** LRU ***
            rpl = strcmp(lru, replacement_policy);
            if (rpl == 0) { // It's LRU
                p_lAc = findLessAccessTSset(cache1, index1, associativity);
                cache1->Cache_Upper[index1][p_lAc] = line1;     // line1 = upper
                cache1->Cache_Data[index1][p_lAc] = DATA;
                cache1->T_Access[index1][p_lAc] = currently_clk;  // Update T_Access
                cache1->T_Load[index1][p_lAc]   = currently_clk ; // Update T_Load
            }
            // *** FIFO ***
            rpf = strcmp(fifo, replacement_policy);
            if (rpf == 0) { // It's LRU
                p_lLd = findLessLoadTSset(cache1, index1, associativity);
                cache1->Cache_Upper[index1][p_lLd] = line1;
                cache1->Cache_Data[index1][p_lLd] = DATA;
                cache1->T_Access[index1][p_lLd] = currently_clk; // Update T_Access
                cache1->T_Load[index1][p_lLd] = currently_clk;   // Update T_Load
            }

        }
        /**********************************************************************/

        /************************** Set isn't full ****************************/
        else {                  // There are a free block in the set (by index1)
            free_block = random_free_space_set (cache1, index1, associativity);

            cache1->Cache_Upper[index1][free_block] = line1;
            cache1->Cache_Data[index1][free_block] = DATA;
            cache1->T_Access[index1][free_block] = currently_clk; // Update the T_Access
            cache1->T_Load[index1][free_block] = currently_clk;   // Update the T_Load
        }
    }
    /**************************************************************************/

    /****************************** Read Hit ********************************/
    else {               // position with the right upper was found
        /** The position with the upper was found in the passed set (index1)
         *     The T_Access and T_Load is updated in this case, because a number_of_writes
         *     data is writed in a position at the set in cache that already have
         *     an another data.
         */
        result1->read_hits++;
        cache1->Cache_Upper[index1][position_that_has_this_upper] = line1;
        //cache1->Cache_Data[index1][position_that_has_this_upper] = DATA;  // Modified bit
        cache1->T_Access[index1][position_that_has_this_upper] = currently_clk; // Update the T_Access

    }
}

/**
 * This function generates a formated output with the results of cache simulation
 */
void generate_output(Results cache_results, char *output_name){
    FILE *ptr_file_output;
    ptr_file_output=fopen(output_name, "wb");

    if (!ptr_file_output) {
        printf("\nThe file of output can't be writed\n\n");
    }
    else {
        fprintf(ptr_file_output, "Access count:%d\n", cache_results.acess_count);
        fprintf(ptr_file_output, "Read hits:%d\n", cache_results.read_hits);
        fprintf(ptr_file_output, "Read misses:%d\n", cache_results.read_misses);
        fprintf(ptr_file_output, "Write hits:%d\n", cache_results.write_hits);
        fprintf(ptr_file_output, "Write misses:%d\n", cache_results.write_misses);
    }

    fclose(ptr_file_output);                  // Close the output file (results)
}
/******************************************************************************/

int main(int argc, char **argv)               // Files are passed by a parameter
{
    printf("\n*** Cache Simulator ***\n\n");

    Desc    cache_description;
    Results cache_results;
    Cache   cache_mem;   // This contains the data, access and load informations

    // Init of results
    cache_results.acess_count = 0;
    cache_results.read_hits = 0;
    cache_results.read_misses = 0;
    cache_results.write_hits = 0;
    cache_results.write_misses = 0;

    char *description = argv[1];
    char *input       = argv[2];
    char *output_name = argv[3];
    FILE *ptr_file_specs_cache;
    FILE *ptr_file_input;
    char RorW;                  // Read or Write (address)
    int number_of_reads = 0;    // Number of read request operations by the CPU
    int number_of_writes = 0;   // Number of write request operations by the CPU
    /*Informations of each address*/
    long unsigned address;                      // Address passed by the CPU
    int words_per_line;
    int number_of_sets;                         // number_of_lines/associativity
    long unsigned line;
    int index;
    int data = 1;

    /*********************** Cache Description ********************************/
    ptr_file_specs_cache = fopen(description, "rb");

    if (!ptr_file_specs_cache) {
        printf("\nThe file of cache description is unable to open!\n\n");
        return -1;
    }
    else {                                                 // File can be opened
        fscanf(ptr_file_specs_cache, "line size = %d\n", &cache_description.line_size);
        fscanf(ptr_file_specs_cache, "number of lines = %d\n", &cache_description.number_of_lines);
        fscanf(ptr_file_specs_cache, "associativity = %d\n", &cache_description.associativity);
        fscanf(ptr_file_specs_cache, "replacement policy = %s\n", cache_description.replacement_policy);
    }
    fclose(ptr_file_specs_cache);            // Close the cache description file
    // DEBUG prints
    #if DEBUG == 1
    printf("%d\n",   cache_description.line_size);
    printf("%d\n",   cache_description.number_of_lines);
    printf("%d\n",   cache_description.associativity);
    printf("%s\n\n", cache_description.replacement_policy);
    #endif
    /**************************************************************************/

    words_per_line = cache_description.line_size/BYTES_PER_WORD; // 1 byte is the size of a word in this simulator
    number_of_sets = cache_description.number_of_lines / cache_description.associativity;

    /**************** Alloc space for Cache Memory Data ***********************/
    int i;                         // index for the allocation with the loop for
    cache_mem.Cache_Data = malloc( number_of_sets * sizeof(int *));
    for (i=0; i<number_of_sets; i++) {
        cache_mem.Cache_Data[i] = malloc (cache_description.associativity * sizeof(int));
    }
    /**************************************************************************/

    /******************* Alloc space for Cache Upper **************************/
    cache_mem.Cache_Upper = malloc( number_of_sets * sizeof(long unsigned *));
    for (i=0; i<number_of_sets; i++) {
        cache_mem.Cache_Upper[i] = malloc (cache_description.associativity * sizeof(long unsigned));
    }
    /**************************************************************************/

    /**************** Alloc space for Access Time Stamp************************/
    cache_mem.T_Access = malloc( number_of_sets * sizeof(long unsigned *));
    for (i=0; i<number_of_sets; i++) {
        cache_mem.T_Access[i] = malloc (cache_description.associativity * sizeof(long unsigned));
    }
    /**************************************************************************/

    /**************** Alloc space for load Time Stamp************************/
    cache_mem.T_Load = malloc( number_of_sets * sizeof(long unsigned *));
    for (i=0; i<number_of_sets; i++) {
        cache_mem.T_Load[i] = malloc (cache_description.associativity * sizeof(long unsigned));
    }
    /**************************************************************************/

    startCache(&cache_mem, number_of_sets, cache_description.associativity);

    /***************** Input Trace File and simulation ************************/

    ptr_file_input = fopen(input, "rb");
    if (!ptr_file_input) {
        printf("\nThe file of Input is unable to open!\n\n");
        return -1;
    }
    else {
        while (fscanf(ptr_file_input, "%lu %c\n", &address, &RorW) != EOF){
            currently_clk += 1;                                     // Increment the clock
            //printf("ADDRESS: %lu e RW: %c\n", address, RorW);
            cache_results.acess_count++;
            if (RorW == 'R') {
                line = make_upper(address, BYTES_PER_WORD, words_per_line);
                index = make_index (number_of_sets, line);

                // DEBUG prints
                #if DEBUG == 1
                printf("Index: %d\n", index);
                printf("The line: %lu\n", line);
                #endif

                number_of_reads++;
                read_cache(&cache_mem, &cache_results, index, line, data, cache_description.associativity, cache_description.replacement_policy);
            }
            else if (RorW == 'W'){
                line  = make_upper(address, BYTES_PER_WORD, words_per_line);
                index = make_index (number_of_sets, line);
                number_of_writes++;
                write_cache(&cache_mem, &cache_results, index, line, data, cache_description.associativity, cache_description.replacement_policy);
            }
            else {
                printf("\nUndefined operation request detected\n");

            }
        }

        #if DEBUG == 1
        printf("\nR:%d, W:%d\n", number_of_reads, number_of_writes);
        #endif
    }
    /**************************************************************************/

    /****************************** Output ************************************/
    generate_output(cache_results, output_name);
    /**************************************************************************/

    #if CACHEVIEW == 1
    printf("\n\nCurrently Cache\n");
    int m, n, vazio=0;
    for (m=0; m<number_of_sets; m++) {
        for (n=0; n<cache_description.associativity; n++) {
            printf("%lu ", cache_mem.Cache_Upper[m][n]);
            if (cache_mem.Cache_Data[m][n]==0)
                vazio++;
        }
        printf("\n");
    }
    printf ("\nNAO MODIFS: %d\n", vazio);

    printf("\n\nCurrently Cache\n");
    for (m=0; m<number_of_sets; m++) {
        for (n=0; n<cache_description.associativity; n++) {
            printf("%d ", cache_mem.Cache_Data[m][n]);
        }
        printf("\n");
    }
    #endif

    cache_mem.Cache_Data  = NULL;  // "Free" in the memory for the Cache_Data[][]
    cache_mem.Cache_Upper = NULL;  // "Free" in the memory for the Cache_Upper[][]
    cache_mem.T_Access    = NULL;  // "Free" in the memory for the T_Access[][]
    cache_mem.T_Load      = NULL;  // "Free" in the memory for the T_Load[][]

    fclose(ptr_file_input);                 // Close the input file (trace file)

   return 0;
}
