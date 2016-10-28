#include <stdio.h>
int main(int argc, char **argv) // File is passed by a parameter
{   
    int line_size;              // Block size
    int number_of_lines;        // Number of blocks
    int associativity;          // Number of blocks per set
    char replacement_policy[5]; // LRY or FIFO + \0
    char write_policy[13];      // WRITEBACK or WRITETHROUGH + \0
   
    printf("\n*** Cache Simulator ***\n");
    
    char *input = argv[1];
    FILE *ptr_file_specs_cache;
    ptr_file_specs_cache = fopen(input, "rb");
    
    if(!ptr_file_specs_cache) {
        printf("This file is unable to open!");
        return -1;
    }
    else {                      // File can be opened 
        printf("File can be parsed!");
    }

    fclose(ptr_file_specs_cache);

   return 0;
}  
