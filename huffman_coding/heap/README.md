# Huffman Coding

A C implementation of the Huffman coding algorithm, built on top of a
custom Min Binary Heap (priority queue).

## Description

This project implements, in order:

1. A generic **Min Binary Heap** (`heap/`), used as a priority queue:
   - `heap_create` — create an empty heap
   - `binary_tree_node` — create a generic binary tree node
   - `heap_insert` — insert a value, maintaining heap shape and order
   - `heap_extract` — pop and return the smallest value
   - `heap_delete` — free an entire heap
2. **Huffman coding**, built on the heap above:
   - `symbol_create` — pair a character with its frequency
   - `huffman_priority_queue` — build a min heap of symbols, ordered
     by frequency
   - `huffman_extract_and_insert` — merge the two least frequent
     nodes into one internal node and reinsert it
   - `huffman_tree` — repeat the merge above until one root remains,
     producing the Huffman tree
   - `huffman_codes` — walk the tree (0 = left, 1 = right) and print
     each symbol's Huffman code

## Compilation

```
gcc -Wall -Wextra -Werror -pedantic -Iheap/ -I./ heap/*.c *.c -o your_program
```

