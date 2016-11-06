/**
 * --------------------------------------------------------------------------
 * Cache Simulator
 * Levindo Neto (https://github.com/levindoneto/Cache-Simulator)
 * --------------------------------------------------------------------------
 */

// Libraries
#include <stdio.h>
#include "cache_simulator.h"           // Header file with the structs and functions prototypes
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>                  // For the time stamp stuff
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
    printf("\n*** Cache Simulator ***\n");

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
    int address;                                // Address passed by the CPU
    int words_per_line;
    int number_of_sets;                         // number_of_lines/associativity
    int line;
    int tag;
    int index;

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

    // DEBUG prints
    #if DEBUG == 1
    printf("%d\n",   cache_description.line_size);
    printf("%d\n",   cache_description.number_of_lines);
    printf("%d\n",   cache_description.associativity);
    printf("%s\n\n", cache_description.replacement_policy);
    #endif
    /**************************************************************************/

    words_per_line = cache_description.line_size/BYTES_PER_WORD; // 8 is the size of a word in this simulator
    number_of_sets = cache_description.number_of_lines / cache_description.associativity;

    /**************** Alloc space for cache memory data ***********************/

    int i, j;           // index for the allocation with the loop for
    cache_mem.Cache_Data = malloc( number_of_sets * sizeof(int)); // (*cache_mem).Cache_Data
    for (i=0; i<number_of_sets; i++) {
        cache_mem.Cache_Data[i] = malloc (cache_description.number_of_lines * sizeof(int));
        for (j=0; j<cache_description.number_of_lines; j++) {
            cache_mem.Cache_Data[i][j] = malloc(words_per_line * sizeof(int));
        }
    }
    //printf ("co: %d", co);
    /**************************************************************************/

    /**************** Alloc space for Access Time Stamp************************/
    cache_mem.T_Access = malloc( number_of_sets * sizeof(float));
    for (i=0; i<number_of_sets; i++) {
        cache_mem.T_Access[i] = malloc (cache_description.number_of_lines * sizeof(float));
        for (j=0; j<cache_description.number_of_lines; j++) {
            cache_mem.T_Access[i][j] = malloc(words_per_line * sizeof(float));
        }
    }
    /**************************************************************************/

    /***************** Input Trace File and simulation ************************/


    ptr_file_input = fopen(input, "rb");
    if (!ptr_file_input) {
        printf("\nThe file of Input is unable to open!\n\n");
        return -1;
    }
    else {
        while (fscanf(ptr_file_input, "%d %c\n", &address, &RorW) != EOF){
            cache_results.acess_count++;
            if (RorW == 'R') {
                line = make_upper(address, BYTES_PER_WORD, words_per_line);
                index = make_index (number_of_sets, line);

                // DEBUG prints
                #if DEBUG == 1
                //printf("Index: %d\n", index);
                printf("The line: %d\n", line);
                #endif

                number_of_reads++;
                //read(...);
            }
            else if (RorW == 'W'){
                number_of_writes++;
                //write(...);
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
    generate_output(cache_results);
    /**************************************************************************/

    cache_mem.Cache_Data = NULL; // Free in the memory for the Cache[][][]
    cache_mem.T_Access = NULL; // Free in the memory for the T_Access[][][]

    fclose(ptr_file_specs_cache);      // Close the cache description file
    fclose(ptr_file_input);            // Close the input file (trace file)

   return 0;
}
