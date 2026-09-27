#include <stdlib.h>
#include <string.h>
#include "heap.h"

/**
 * get_path - Computes the binary path (root to leaf) locating the
 * n-th node (1-indexed, level-order) of a complete binary tree
 * @n: The 1-indexed position of the node
 * @path: Buffer to store the path ('0' = left, '1' = right)
 *
 * Return: Length of the path (excluding the null terminator)
 */
static int get_path(size_t n, char *path)
{
	char tmp[64];
	int i = 0, j;

	while (n > 0)
	{
		tmp[i++] = (char)((n % 2) + '0');
		n /= 2;
	}
	for (j = 0; j < i / 2; j++)
	{
		char t = tmp[j];

		tmp[j] = tmp[i - 1 - j];
		tmp[i - 1 - j] = t;
	}
	tmp[i] = '\0';
	strcpy(path, tmp + 1);

	return ((int)strlen(path));
}

/**
 * find_node - Walks the full path to find the last node (in level
 * order) of a complete binary tree
 * @root: Root of the tree to walk
 * @path: Path string ('0'/'1') describing the route from the root
 * @len: Length of path
 *
 * Return: Pointer to the located node
 */
static binary_tree_node_t *find_node(binary_tree_node_t *root,
	char *path, int len)
{
	int i;
	binary_tree_node_t *cur = root;

	for (i = 0; i < len; i++)
		cur = (path[i] == '0') ? cur->left : cur->right;

	return (cur);
}

/**
 * sift_down - Bubbles the root's data down the heap by swapping
 * data (not node structure) until the heap property is restored
 * @heap: The heap being repaired
 * @node: Node to start sifting down from (usually the root)
 */
static void sift_down(heap_t *heap, binary_tree_node_t *node)
{
	binary_tree_node_t *smallest;
	void *tmp;

	while (1)
	{
		smallest = node;
		if (node->left && heap->data_cmp(node->left->data,
			smallest->data) < 0)
			smallest = node->left;
		if (node->right && heap->data_cmp(node->right->data,
			smallest->data) < 0)
			smallest = node->right;
		if (smallest == node)
			break;
		tmp = node->data;
		node->data = smallest->data;
		smallest->data = tmp;
		node = smallest;
	}
}

/**
 * heap_extract - Extracts the root value of a Min Binary Heap
 * @heap: Pointer to the heap to extract from
 *
 * Return: Pointer to the data that was stored in the root, or NULL
 * on failure
 */
void *heap_extract(heap_t *heap)
{
	void *extracted;
	char path[64];
	int len;
	binary_tree_node_t *last;

	if (heap == NULL || heap->root == NULL)
		return (NULL);

	extracted = heap->root->data;

	if (heap->size == 1)
	{
		free(heap->root);
		heap->root = NULL;
		heap->size = 0;
		return (extracted);
	}

	len = get_path(heap->size, path);
	last = find_node(heap->root, path, len);

	heap->root->data = last->data;

	if (last->parent->left == last)
		last->parent->left = NULL;
	else
		last->parent->right = NULL;

	free(last);
	heap->size--;

	sift_down(heap, heap->root);

	return (extracted);
}
