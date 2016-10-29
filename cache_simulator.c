// Libraries
#include <stdio.h>

int main(int argc, char **argv)  // Files are passed by a parameter
{   
    typedef struct desc {
        int line_size;               // Block size
        int number_of_lines;         // Number of blocks
        int associativity;           // Number of blocks per set
        char replacement_policy[5];  // LRY or FIFO + \0
    } Desc;
    
    Desc cache_description;
    
    char buffer[100];
    char c[] = "this is tutorialspoint";
    int tes=0;

    printf("\n*** Cache Simulator ***\n");
    
    char *description = argv[1];
    FILE *ptr_file_specs_cache;
    ptr_file_specs_cache = fopen(description, "rb");
    int desc_line;
    
    if(!ptr_file_specs_cache) {
        printf("This file is unable to open!");
        return -1;
    }
    else {                       // File can be opened 
        fscanf(ptr_file_specs_cache, "line size = %d\n", &cache_description.line_size);
        fscanf(ptr_file_specs_cache, "number of lines = %d\n", &cache_description.number_of_lines);
        fscanf(ptr_file_specs_cache, "associativity = %d\n", &cache_description.associativity);
        fscanf(ptr_file_specs_cache, "replacement policy = %s\n", cache_description.replacement_policy);    
    }
    
    printf("%d\n", cache_description.line_size);
    printf("%d\n", cache_description.number_of_lines);
    printf("%d\n", cache_description.associativity);
    printf("%s\n", cache_description.replacement_policy);
    
    fclose(ptr_file_specs_cache); // Close de cache description file

   return 0;
}  