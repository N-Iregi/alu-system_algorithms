#include <stdlib.h>
#include <string.h>
#include "huffman_file.h"

#define MAX_CODE_BITS 256

/**
 * struct hnode_s - Node of the (in-memory, compression-time) Huffman
 * tree, built over the byte values actually present in the input
 *
 * @is_leaf: 1 if this node represents a single byte value
 * @byte: The byte value (meaningful only when is_leaf is set)
 * @freq: Combined frequency of everything under this node
 * @left: Left child (0 bit)
 * @right: Right child (1 bit)
 */
typedef struct hnode_s
{
	int is_leaf;
	unsigned char byte;
	unsigned long freq;
	struct hnode_s *left;
	struct hnode_s *right;
} hnode_t;

/**
 * struct code_s - A symbol's Huffman code, packed MSB-first
 *
 * @bits: Packed code bits, enough room for MAX_CODE_BITS bits
 * @length: Number of meaningful bits in bits
 */
typedef struct code_s
{
	unsigned char bits[MAX_CODE_BITS / 8];
	int length;
} code_t;

/**
 * make_leaf - Allocates a leaf node for one byte value
 * @byte: The byte value
 * @freq: Its frequency in the input
 *
 * Return: Pointer to the new node, or NULL on failure
 */
static hnode_t *make_leaf(unsigned char byte, unsigned long freq)
{
	hnode_t *node = malloc(sizeof(hnode_t));

	if (node == NULL)
		return (NULL);
	node->is_leaf = 1;
	node->byte = byte;
	node->freq = freq;
	node->left = NULL;
	node->right = NULL;
	return (node);
}

/**
 * make_internal - Allocates an internal node merging two subtrees
 * @left: Left subtree (0 bit)
 * @right: Right subtree (1 bit)
 *
 * Return: Pointer to the new node, or NULL on failure
 */
static hnode_t *make_internal(hnode_t *left, hnode_t *right)
{
	hnode_t *node = malloc(sizeof(hnode_t));

	if (node == NULL)
		return (NULL);
	node->is_leaf = 0;
	node->byte = 0;
	node->freq = left->freq + right->freq;
	node->left = left;
	node->right = right;
	return (node);
}

/**
 * free_tree - Recursively frees a Huffman tree
 * @node: Root of the (sub)tree to free
 */
static void free_tree(hnode_t *node)
{
	if (node == NULL)
		return;
	free_tree(node->left);
	free_tree(node->right);
	free(node);
}

/**
 * build_tree - Repeatedly merges the two least frequent remaining
 * nodes until a single Huffman tree remains
 * @nodes: Array of leaf nodes (consumed/reordered in place)
 * @n: Number of leaves
 *
 * Return: Root of the built tree, or NULL on failure
 */
static hnode_t *build_tree(hnode_t **nodes, int n)
{
	int i, min1, min2, tmp;
	hnode_t *merged;

	while (n > 1)
	{
		min1 = 0;
		for (i = 1; i < n; i++)
			if (nodes[i]->freq < nodes[min1]->freq)
				min1 = i;

		min2 = (min1 == 0) ? 1 : 0;
		for (i = 0; i < n; i++)
			if (i != min1 && nodes[i]->freq < nodes[min2]->freq)
				min2 = i;

		merged = make_internal(nodes[min1], nodes[min2]);
		if (merged == NULL)
			return (NULL);

		if (min1 > min2)
		{
			tmp = min1;
			min1 = min2;
			min2 = tmp;
		}
		nodes[min1] = merged;
		nodes[min2] = nodes[n - 1];
		n--;
	}
	return (nodes[0]);
}

/**
 * set_code_bit - Sets or clears one bit within a packed code buffer
 * @code: The code being built
 * @index: Bit position (0 = first bit emitted from the root)
 * @value: 0 or 1
 */
static void set_code_bit(code_t *code, int index, int value)
{
	int byte_idx = index / 8, bit_idx = 7 - (index % 8);

	if (value)
		code->bits[byte_idx] |= (unsigned char)(1 << bit_idx);
	else
		code->bits[byte_idx] &= (unsigned char)(~(1 << bit_idx));
}

/**
 * assign_codes - Walks the tree, recording each leaf's code
 * @node: Current node
 * @path: Scratch buffer of 0/1 values along the current path
 * @depth: Current depth (length of path so far)
 * @codes: Output table of codes, indexed by byte value
 */
static void assign_codes(hnode_t *node, unsigned char *path, int depth,
	code_t codes[256])
{
	int i;

	if (node == NULL)
		return;

	if (node->is_leaf)
	{
		codes[node->byte].length = depth;
		memset(codes[node->byte].bits, 0, sizeof(codes[node->byte].bits));
		for (i = 0; i < depth; i++)
			set_code_bit(&codes[node->byte], i, path[i]);
		return;
	}

	path[depth] = 0;
	assign_codes(node->left, path, depth + 1, codes);
	path[depth] = 1;
	assign_codes(node->right, path, depth + 1, codes);
}

/**
 * write_header - Writes the magic, sizes and per-symbol code table
 * @out: File to write to
 * @original_size: Size of the uncompressed input, in bytes
 * @byte_val: Distinct byte values present in the input
 * @count: Number of entries in byte_val
 * @codes: Code table indexed by byte value (unused when count < 2)
 *
 * Return: 1 on success, 0 on failure
 */
static int write_header(FILE *out, unsigned long original_size,
	unsigned char *byte_val, int count, code_t codes[256])
{
	int i, nbytes;

	if (fwrite(HUFFMAN_MAGIC, 1, HUFFMAN_MAGIC_LEN, out) != HUFFMAN_MAGIC_LEN)
		return (0);
	if (!write_u32(out, original_size) || !write_u16(out, (unsigned int)count))
		return (0);

	for (i = 0; i < count; i++)
	{
		unsigned char b = byte_val[i];
		int length = (count == 1) ? 0 : codes[b].length;

		if (fputc(b, out) == EOF || fputc(length, out) == EOF)
			return (0);
		if (length > 0)
		{
			nbytes = (length + 7) / 8;
			if ((int)fwrite(codes[b].bits, 1, (size_t)nbytes, out) != nbytes)
				return (0);
		}
	}
	return (1);
}

/**
 * compress_file - Compresses in into out using Huffman coding
 * @in: Already-open input file (binary mode)
 * @out: Already-open, empty output file (binary mode)
 *
 * Return: 1 on success, 0 on failure
 */
int compress_file(FILE *in, FILE *out)
{
	unsigned char *buffer;
	unsigned long size, freq[256] = {0};
	unsigned char byte_val[256];
	int count = 0, i;
	code_t codes[256];
	hnode_t *leaves[256];
	hnode_t *root;
	bitwriter_t bw;

	if (fseek(in, 0, SEEK_END) != 0)
		return (0);
	size = (unsigned long)ftell(in);
	rewind(in);

	buffer = size > 0 ? malloc(size) : NULL;
	if (size > 0 && buffer == NULL)
		return (0);
	if (size > 0 && fread(buffer, 1, size, in) != size)
	{
		free(buffer);
		return (0);
	}

	for (i = 0; i < (int)size; i++)
		freq[buffer[i]]++;
	for (i = 0; i < 256; i++)
		if (freq[i] > 0)
			byte_val[count++] = (unsigned char)i;

	if (count < 2)
	{
		if (!write_header(out, size, byte_val, count, codes))
		{
			free(buffer);
			return (0);
		}
		free(buffer);
		return (1);
	}

	for (i = 0; i < count; i++)
	{
		leaves[i] = make_leaf(byte_val[i], freq[byte_val[i]]);
		if (leaves[i] == NULL)
		{
			free(buffer);
			return (0);
		}
	}

	root = build_tree(leaves, count);
	if (root == NULL)
	{
		free(buffer);
		return (0);
	}

	if (root->is_leaf)
	{
		/* Only reachable if count == 1, already handled above. */
		free_tree(root);
		free(buffer);
		return (0);
	}

	{
		unsigned char path[MAX_CODE_BITS];

		assign_codes(root, path, 0, codes);
	}
	free_tree(root);

	if (!write_header(out, size, byte_val, count, codes))
	{
		free(buffer);
		return (0);
	}

	bw_init(&bw, out);
	for (i = 0; i < (int)size; i++)
	{
		unsigned char b = buffer[i];
		int j;

		for (j = 0; j < codes[b].length; j++)
		{
			int byte_idx = j / 8, bit_idx = 7 - (j % 8);

			bw_put_bit(&bw, (codes[b].bits[byte_idx] >> bit_idx) & 1);
		}
	}
	bw_flush(&bw);

	free(buffer);
	return (1);
}
