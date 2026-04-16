HW3 - Cache Simulator
Author: Andre Mercado

Build
g++ cache_sim.cpp -o cache_sim -std=c++17

Run
./cache_sim num_entries associativity memory_reference_file [block_size [l2_entries l2_assoc]]

- num_entries: total number of cache entries
- associativity: number of ways per set
- memory_reference_file: file with space-separated word addresses
- block_size (optional): number of words per block, default is 1
- l2_entries l2_assoc (optional): enable L2 cache with given size and associativity

Output is written to cache_sim_output.

Examples
Create input file:
printf "1 3 5 1 3 1" > memory_reference_file

Basic:
./cache_sim 4 2 memory_reference_file

With block size:
./cache_sim 4 2 memory_reference_file 2

With L2 cache:
./cache_sim 4 2 memory_reference_file 2 16 4

Miss types
COMPULSORY - first time this block was accessed
CAPACITY   - cache is full, would miss even in fully-associative cache
CONFLICT   - caused by set conflicts, would hit in fully-associative cache

Known Bugs / Limitations
No known bugs
