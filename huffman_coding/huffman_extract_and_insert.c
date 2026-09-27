#include <stdlib.h>
#include "huffman.h"

/**
 * huffman_extract_and_insert - Extracts the two least frequent
 * nodes of a priority queue, merges them under a new internal node
 * whose frequency is their sum, and inserts that node back
 * @priority_queue: Pointer to the priority queue to work on
 *
 * Return: 1 on success, 0 on failure
 */
int huffman_extract_and_insert(heap_t *priority_queue)
{
	binary_tree_node_t *left, *right, *parent;
	symbol_t *sym1, *sym2, *new_symbol;

	if (priority_queue == NULL || priority_queue->size < 2)
		return (0);

	left = heap_extract(priority_queue);
	if (left == NULL)
		return (0);

	right = heap_extract(priority_queue);
	if (right == NULL)
		return (0);

	sym1 = left->data;
	sym2 = right->data;

	new_symbol = symbol_create(-1, sym1->freq + sym2->freq);
	if (new_symbol == NULL)
		return (0);

	parent = binary_tree_node(NULL, new_symbol);
	if (parent == NULL)
	{
		free(new_symbol);
		return (0);
	}

	parent->left = left;
	parent->right = right;
	left->parent = parent;
	right->parent = parent;

	if (heap_insert(priority_queue, parent) == NULL)
		return (0);

	return (1);
}
