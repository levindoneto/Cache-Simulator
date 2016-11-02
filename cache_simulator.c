// Libraries
#include <stdio.h>
#include "cache_simulator.h"
#include <stdlib.h>
#include <string.h>
#define DEBUG 0    // 0: FALSE, 1:TRUE

// Prototype of used functions
void generate_output(Results cache_results);

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

    fclose(ptr_file_output);          // Close the output file (results)
}

int main(int argc, char **argv)      // Files are passed by a parameter
{
    Desc   cache_description;
    Results cache_results;

    // Init of results
    cache_results.acess_count = 0;
    cache_results.read_hits = 0;
    cache_results.read_misses = 0;
    cache_results.write_hits = 0;
    cache_results.write_misses = 0;

    printf("\n*** Cache Simulator ***\n");

    char *description = argv[1];
    char *input = argv[2];
    FILE *ptr_file_specs_cache;
    FILE *ptr_file_input;
    FILE *ptr_file_output;
    int address;                // Address passed by the CPU
    char RorW;                  // Read or Write (address)

    ptr_file_input = fopen(input, "rb");
    if (!ptr_file_input) {
        printf("\nThe file of Input is unable to open!\n\n");
        return -1;
    }
    else {
        while (fscanf(ptr_file_input, "%d %c\n", &address, &RorW) != EOF){
            cache_results.acess_count++;
        }
    }

    ptr_file_specs_cache = fopen(description, "rb");
    int desc_line;

    if (!ptr_file_specs_cache) {
        printf("\nThe file of cache description is unable to open!\n\n");
        return -1;
    }
    else {                            // File can be opened
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

    // write(cache_results.things)
    generate_output(cache_results);


    fclose(ptr_file_specs_cache);     // Close the cache description file
    fclose(ptr_file_input);           // Close the input file (trace file)

   return 0;
}
