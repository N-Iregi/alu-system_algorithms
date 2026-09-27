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
 * find_insertion_parent - Walks the path (all but the last step) to
 * find the parent under which the new node must be attached
 * @root: Root of the tree to walk
 * @path: Path string ('0'/'1') describing the route from the root
 * @len: Length of path
 *
 * Return: Pointer to the parent node
 */
static binary_tree_node_t *find_insertion_parent(binary_tree_node_t *root,
	char *path, int len)
{
	int i;
	binary_tree_node_t *cur = root;

	for (i = 0; i < len - 1; i++)
		cur = (path[i] == '0') ? cur->left : cur->right;

	return (cur);
}

/**
 * sift_up - Bubbles a newly inserted node up the heap by swapping
 * data (not node structure) until the heap property is restored
 * @heap: The heap the node belongs to
 * @node: The newly inserted node
 *
 * Return: Pointer to the node now holding the inserted data
 */
static binary_tree_node_t *sift_up(heap_t *heap, binary_tree_node_t *node)
{
	void *tmp;
	binary_tree_node_t *parent = node->parent;

	while (parent != NULL && heap->data_cmp(node->data, parent->data) < 0)
	{
		tmp = node->data;
		node->data = parent->data;
		parent->data = tmp;
		node = parent;
		parent = node->parent;
	}

	return (node);
}

/**
 * heap_insert - Inserts a value in a Min Binary Heap
 * @heap: Pointer to the heap in which to insert
 * @data: Pointer to the data to store in the new node
 *
 * Return: Pointer to the node now holding data, or NULL on failure
 */
binary_tree_node_t *heap_insert(heap_t *heap, void *data)
{
	char path[64];
	int len;
	binary_tree_node_t *parent, *new_node;

	if (heap == NULL || data == NULL)
		return (NULL);

	heap->size++;

	if (heap->root == NULL)
	{
		heap->root = binary_tree_node(NULL, data);
		if (heap->root == NULL)
		{
			heap->size--;
			return (NULL);
		}
		return (heap->root);
	}

	len = get_path(heap->size, path);
	parent = find_insertion_parent(heap->root, path, len);

	new_node = binary_tree_node(parent, data);
	if (new_node == NULL)
	{
		heap->size--;
		return (NULL);
	}

	if (path[len - 1] == '0')
		parent->left = new_node;
	else
		parent->right = new_node;

	return (sift_up(heap, new_node));
}
