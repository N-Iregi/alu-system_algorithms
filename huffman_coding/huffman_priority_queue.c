#include <stdlib.h>
#include "huffman.h"

/**
 * freq_cmp - Compares two priority queue nodes by the frequency of
 * the symbol stored in their nested leaf node
 * @p1: First heap node's data (a binary_tree_node_t *)
 * @p2: Second heap node's data (a binary_tree_node_t *)
 *
 * Return: Difference between the two frequencies
 */
static int freq_cmp(void *p1, void *p2)
{
	symbol_t *s1 = ((binary_tree_node_t *)p1)->data;
	symbol_t *s2 = ((binary_tree_node_t *)p2)->data;

	return ((int)(s1->freq - s2->freq));
}

/**
 * huffman_priority_queue - Creates a priority queue for the Huffman
 * coding algorithm
 * @data: Array of characters
 * @freq: Array of associated frequencies
 * @size: Size of both data and freq arrays
 *
 * Return: Pointer to the created min heap, or NULL on failure
 */
heap_t *huffman_priority_queue(char *data, size_t *freq, size_t size)
{
	heap_t *priority_queue;
	symbol_t *symbol;
	binary_tree_node_t *leaf;
	size_t i;

	if (data == NULL || freq == NULL || size == 0)
		return (NULL);

	priority_queue = heap_create(freq_cmp);
	if (priority_queue == NULL)
		return (NULL);

	for (i = 0; i < size; i++)
	{
		symbol = symbol_create(data[i], freq[i]);
		if (symbol == NULL)
			return (NULL);

		leaf = binary_tree_node(NULL, symbol);
		if (leaf == NULL)
		{
			free(symbol);
			return (NULL);
		}

		if (heap_insert(priority_queue, leaf) == NULL)
			return (NULL);
	}

	return (priority_queue);
}
