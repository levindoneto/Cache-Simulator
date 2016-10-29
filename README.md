# Cache-Simulator 

The cache simulator is used to simulate substitutions using replacement policies (FIFO and LRU) and write back with write allocate policy. The words (blocks/lines) have 1B.

## Specification file content (input)
* Block size (unsigned power 2)
* Total of blocks (unsigned power 2)
* Associativity (unsigned power 2), this is the number of blocks per set
* Replacement policy("LRU" or "FIFO")
* Write policy ("WRITEBACK" or "WRITETHROUGH")

## Output file
* Number of access
* Number of write hits
* Number of write misses
* Number of read hits
* Number of read misses

## How to use
```Terminal
make all
./243685 "cachedesc.bin"
```
