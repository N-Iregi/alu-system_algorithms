#include <stdlib.h>
#include "heap.h"

/**
 * delete_node - Recursively frees a binary tree
 * @node: Root of the (sub)tree to free
 * @free_data: Function used to free a node's data, or NULL
 */
static void delete_node(binary_tree_node_t *node, void (*free_data)(void *))
{
	if (node == NULL)
		return;

	delete_node(node->left, free_data);
	delete_node(node->right, free_data);

	if (free_data != NULL)
		free_data(node->data);

	free(node);
}

/**
 * heap_delete - Deallocates a heap
 * @heap: Pointer to the heap to delete
 * @free_data: Function used to free the content of a node, or NULL
 */
void heap_delete(heap_t *heap, void (*free_data)(void *))
{
	if (heap == NULL)
		return;

	delete_node(heap->root, free_data);
	free(heap);
}
