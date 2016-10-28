#include <stdio.h>
int main()  
{   
    int line_size;
    int number_of_lines;
    int associativity;
    char replacement_policy[5]; // LRY or FIFO + \0
    char write_policy[13]; // WRITEBACK or WRITETHROUGH + \0
   
    printf("\n*** Cache Simulator ***\n");
    
    FILE *ptr_file_specs_cache;
    ptr_file_specs_cache = fopen("cachedesc.bin", "rb");
    
    if(!ptr_file_specs_cache) {
        printf("This file is unable to open!");
        return -1;
    }

    fclose(ptr_file_specs_cache);

   return 0;
}  
