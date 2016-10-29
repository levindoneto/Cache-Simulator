// Libraries
#include <stdio.h>

int main(int argc, char **argv)  // Files are passed by a parameter
{   
    int line_size;               // Block size
    int number_of_lines;         // Number of blocks
    int associativity;           // Number of blocks per set
    char replacement_policy[5];  // LRY or FIFO + \0
    char write_policy[13];       // WRITEBACK or WRITETHROUGH + \0
    
    int c;
    int tes=0;

    printf("\n*** Cache Simulator ***\n");
    
    char *description = argv[1];
    FILE *ptr_file_specs_cache;
    ptr_file_specs_cache = fopen(description, "rb");
    
    if(!ptr_file_specs_cache) {
        printf("This file is unable to open!");
        return -1;
    }
    else {                       // File can be opened 
        while( (c=fgetc(ptr_file_specs_cache)) != EOF ) {        
            printf("palmeiras\n");
            printf("%c", c);
            tes++;
        }

    }

    fclose(ptr_file_specs_cache); // Close de cache description file

   return 0;
}  