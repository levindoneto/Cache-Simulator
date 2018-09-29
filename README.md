# Cache Simulator

__Author:__ Levindo Gabriel Taschetto Neto.

The cache simulator is used to simulate substitutions using replacement policies (FIFO and LRU) and write back with write allocate policy. The words (blocks/lines) have 1B.

## Specification file content (input)
* Block size (unsigned power 2)
* Total of blocks (unsigned power 2)
* Associativity (unsigned power 2), this is the number of blocks per set
* Replacement policy("LRU" or "FIFO")

## Output file
* Number of access
* Number of write hits
* Number of write misses
* Number of read hits
* Number of read misses

## Cache Structure
Cache is a struct that contains four information:
* Data_Cache
* Upper (used as a tag in the set)
* T_Access (used in the LRU algorithm)
* T_Load (used in the LRU algorithm)

These informations are modeled as a dynamic array 2-D which contains the following positions:
* Set (number_of_lines / associativity)
* Line (associativity)

To access a data in the cache, the only thing that is necessary is the set, which is the information about "index" in an address.
To access the line is used an "upper" information. The upper is used only for comparisons to know which line (block) should be accessed because the set doesn't have any particular order.

## How to use
```Terminal
make all
./243685 "Tests/Cache_Descriptions/cachedescriptionfile.format" "Tests/Trace_Files/inputfile.format" "outputfile.format"
```

## License

MIT License. Click [here](LICENSE.md) for more information about this license.
