#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"

/**
 * print_codes - Recursively walks the Huffman tree, printing the
 * code (path of 0s and 1s from the root) of each leaf symbol
 * @node: Current node being visited
 * @code: Buffer accumulating the code so far
 * @depth: Current depth (also the length of code so far)
 */
static void print_codes(binary_tree_node_t *node, char *code, int depth)
{
	symbol_t *symbol;

	if (node == NULL)
		return;

	if (node->left == NULL && node->right == NULL)
	{
		symbol = node->data;
		code[depth] = '\0';
		printf("%c: %s\n", symbol->data, code);
		return;
	}

	code[depth] = '0';
	print_codes(node->left, code, depth + 1);

	code[depth] = '1';
	print_codes(node->right, code, depth + 1);
}

/**
 * free_tree - Recursively frees a Huffman tree, including the
 * symbol_t stored in each node
 * @node: Root of the (sub)tree to free
 */
static void free_tree(binary_tree_node_t *node)
{
	if (node == NULL)
		return;

	free_tree(node->left);
	free_tree(node->right);
	free(node->data);
	free(node);
}

/**
 * huffman_codes - Builds the Huffman tree for a set of symbols and
 * prints the resulting Huffman code for each one
 * @data: Array of characters
 * @freq: Array of associated frequencies
 * @size: Size of both data and freq arrays
 *
 * Return: 1 on success, 0 on failure
 */
int huffman_codes(char *data, size_t *freq, size_t size)
{
	binary_tree_node_t *root;
	char code[256];

	if (data == NULL || freq == NULL || size == 0)
		return (0);

	root = huffman_tree(data, freq, size);
	if (root == NULL)
		return (0);

	print_codes(root, code, 0);

	free_tree(root);

	return (1);
}
