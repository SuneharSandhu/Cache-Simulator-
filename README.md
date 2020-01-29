# Cache-Simulator

This is a simple cache simulator that calculates the number of memory writes and memory reads for a memory access pattern provided as a file. The inputs to the program, aside from the file listing the memory accesses, are parameters that specify the cache to be simulated.

Usage: sim <cache size> <associativity> <block size> <replacement algorithm> <write policy> <tracefile.txt>

More information about arguments:

cache size: must be a power of 2 integer represented in decimal(base 10)

associativity:
  
  - direct  - simulates of a direct-mapped cache
  - assoc:n - simulates an n-way associative cache where n is an integer that is a power of 2
  
block size : must be a power of 2 integer represented in decimal(base 10)
  
replacement algorithm:
  
  - FIFO - simulates first-in-first-out replacement
  - LRU - simulates least-recently-used replacement
  
write policy: -wt - simulates a write-through cache
  
tracefile: Must be a text file with the memory access trace of a program. 
