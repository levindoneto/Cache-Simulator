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
#define BYTES_PER_WORD 1               // All words with 1 Byte (bytes_per_word)

/**************************** Functions ***************************************/
/**
 * That function get the line (upper) of a given address by the CPU
 * This upper is equal to the Tag information
 */
int make_upper(int address, int words_per_line, int bytes_per_word) {
    int line;
    line = ((address/words_per_line))/bytes_per_word;
    return line;
}

/**
 * That function generates the index for the set in the cache
 */
int make_index (int number_of_sets, int upper) {
    int index;
    index = upper % number_of_sets;

    return index;                               // index of the set in the cache
}

int getPosUpper (Cache cache, int index, int line, int associativity) {
    int block_i;
    for (block_i=0; block_i<associativity; block_i++) {
        if (cache.Cache_Upper[index][block_i] == line){
            return block_i;               // Upper inside of the set found
        }
    }
    return -1;                              // Upper inside of the set not found
}

void write_cache (Cache cache1, int index1, int line1, int data1, int associativity) {
    // Writing the data in the set (by index) in the position that contains the
    //     upper (by line)
    int position_that_has_this_upper = getPosUpper(cache1, index1, line1, associativity);
    //cache1.Cache_Upper
    //Dirty_Bit

    struct timeval tv;
    gettimeofday(&tv, NULL);
    cache1.T_Access[index1][position_that_has_this_upper] = tv.tv_usec;
    //cache1.T_Load
    cache1.Cache_Data[index1][position_that_has_this_upper] = data1;
    //printf("\n>>>LOOK ME: %d\n", cache1.Cache_Data[index1][position_that_has_this_upper]);
}

int read_cache (Cache cache1, int index1, int line1, int data1, int associativity) {
    int position_that_has_this_upper = getPosUpper(cache1, index1, line1, associativity);
    //cache1.Cache_Upper
    //Dirty_Bit

    struct timeval tv;
    gettimeofday(&tv, NULL);
    cache1.T_Access[index1][position_that_has_this_upper] = tv.tv_usec;
    //cache1.T_Load
    return cache1.Cache_Data[index1][position_that_has_this_upper];
}

/**
 * That function generates a formated output with the results of cache simulation
 */
void generate_output(Results cache_results){
    FILE *ptr_file_output;
    ptr_file_output=fopen("output.out", "wb");
    //char output_file[11] = "output.out";

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

    /* Getting time stamp in microseconds (used in LRU and FIFO replacement policy) */
    struct timeval tv;
    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_usec;

    //printf("TEMPO: %ld\n", (long int) curtime);

    sleep(2);
    gettimeofday(&tv, NULL);
    curtime=tv.tv_usec;
    //printf("TEMPO 2: %ld\n", (long int) curtime);
    /**************************************************************************/

    printf("\n*** Cache Simulator ***\n");
    //printf("TIME1%lld\n", (long long) time(NULL));

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
    char *input = argv[2];
    FILE *ptr_file_specs_cache;
    FILE *ptr_file_input;
    FILE *ptr_file_output;
    char RorW;                  // Read or Write (address)
    int number_of_reads = 0;    // Number of read request operations by the CPU
    int number_of_writes = 0;   // Number of write request operations by the CPU
    /*Informations of each address*/
    long unsigned address;                                // Address passed by the CPU
    int words_per_line;
    int number_of_sets;                         // number_of_lines/associativity
    long unsigned line;
    int tag;
    int index;
    int data = 1;

    /*********************** Cache Description ********************************/
    ptr_file_specs_cache = fopen(description, "rb");
    int desc_line;

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
    fclose(ptr_file_specs_cache);           // Close the cache description file
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
    int i, j;           // index for the allocation with the loop for
    cache_mem.Cache_Data = malloc( number_of_sets * sizeof(int));
    for (i=0; i<number_of_sets; i++) {
        cache_mem.Cache_Data[i] = malloc (cache_description.associativity * sizeof(int));
    }
    /**************************************************************************/

    /******************* Alloc space for Cache Upper **************************/
    cache_mem.Cache_Upper = malloc( number_of_sets * sizeof(long unsigned));
    for (i=0; i<number_of_sets; i++) {
        cache_mem.Cache_Upper[i] = malloc (cache_description.associativity * sizeof(long unsigned));
    }
    /**************************************************************************/

    /******************** Alloc space for Dirty Bit **************************
    cache_mem.Dirty_Bit = malloc( number_of_sets * sizeof(int));
    for (i=0; i<number_of_sets; i++) {
        cache_mem.Dirty_Bit[i] = malloc (cache_description.associativity * sizeof(int));
    }
    *************************************************************************/

    /**************** Alloc space for Access Time Stamp************************/
    cache_mem.T_Access = malloc( number_of_sets * sizeof(time_t));
    for (i=0; i<number_of_sets; i++) {
        cache_mem.T_Access[i] = malloc (cache_description.associativity * sizeof(time_t));
    }
    /**************************************************************************/

    /**************** Alloc space for load Time Stamp************************/
    cache_mem.T_Load = malloc( number_of_sets * sizeof(time_t));
    for (i=0; i<number_of_sets; i++) {
        cache_mem.T_Load[i] = malloc (cache_description.associativity * sizeof(time_t));
    }
    /**************************************************************************/

    /***************** Input Trace File and simulation ************************/


    ptr_file_input = fopen(input, "rb");
    if (!ptr_file_input) {
        printf("\nThe file of Input is unable to open!\n\n");
        return -1;
    }
    else {
        while (fscanf(ptr_file_input, "%lu %c\n", &address, &RorW) != EOF){
            cache_results.acess_count++;
            if (RorW == 'R') {
                line = make_upper(address, BYTES_PER_WORD, words_per_line);
                index = make_index (number_of_sets, line);

                // DEBUG prints
                #if DEBUG == 0
                //printf("Index: %d\n", index);
                printf("The line: %lu\n", line);
                #endif

                number_of_reads++;
                //read_cache(cache_mem, index, line, data, cache_description.associativity);
            }
            else if (RorW == 'W'){
                line  = make_upper(address, BYTES_PER_WORD, words_per_line);
                index = make_index (number_of_sets, line);
                number_of_writes++;
                //write_cache(cache_mem, index, line, data, cache_description.associativity);
            }
            else {
                printf("\nUndefined operation request detected\n");

            }
        }

        // Initial tests
          // address=2147483647

        #if DEBUG == 1
        printf("\nR:%d, W:%d\n", number_of_reads, number_of_writes);
        #endif
    }
    /**************************************************************************/

    /****************************** Output ************************************/
    generate_output(cache_results);
    /**************************************************************************/

    cache_mem.Cache_Data  = NULL;  // "Free" in the memory for the Cache_Data[][]
    cache_mem.Cache_Upper = NULL;  // "Free" in the memory for the Cache_Upper[][]
    //cache_mem.Dirty_Bit   = NULL;  // "Free" in the memory for the Dirty_Bit[][]
    cache_mem.T_Access    = NULL;  // "Free" in the memory for the T_Access[][]
    cache_mem.T_Load      = NULL;  // "Free" in the memory for the T_Load[][]


    fclose(ptr_file_input);                 // Close the input file (trace file)

   return 0;
}
