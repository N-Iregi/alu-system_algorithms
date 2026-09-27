#include <stdlib.h>
#include <string.h>
#include "huffman_file.h"

/**
 * struct trienode_s - Node of the (decompression-time) decode trie,
 * built directly from the codes stored in the file header
 *
 * @is_leaf: 1 once this node has been marked as a code's endpoint
 * @byte: The byte value this leaf decodes to
 * @left: Child reached on a 0 bit
 * @right: Child reached on a 1 bit
 */
typedef struct trienode_s
{
	int is_leaf;
	unsigned char byte;
	struct trienode_s *left;
	struct trienode_s *right;
} trienode_t;

/**
 * trie_new_node - Allocates a zeroed internal trie node
 *
 * Return: Pointer to the new node, or NULL on failure
 */
static trienode_t *trie_new_node(void)
{
	trienode_t *node = calloc(1, sizeof(trienode_t));

	return (node);
}

/**
 * free_trie - Recursively frees a decode trie
 * @node: Root of the (sub)trie to free
 */
static void free_trie(trienode_t *node)
{
	if (node == NULL)
		return;
	free_trie(node->left);
	free_trie(node->right);
	free(node);
}

/**
 * trie_insert - Inserts one symbol's code into the decode trie
 * @root: Trie root
 * @bits: Packed code bits (MSB-first), as read from the header
 * @length: Number of meaningful bits in bits
 * @byte_val: The byte value this code decodes to
 *
 * Return: 1 on success, 0 on failure (allocation failure)
 */
static int trie_insert(trienode_t *root, unsigned char *bits, int length,
	unsigned char byte_val)
{
	trienode_t *cur = root;
	int i, bit;

	for (i = 0; i < length; i++)
	{
		bit = (bits[i / 8] >> (7 - (i % 8))) & 1;
		if (bit == 0)
		{
			if (cur->left == NULL)
			{
				cur->left = trie_new_node();
				if (cur->left == NULL)
					return (0);
			}
			cur = cur->left;
		}
		else
		{
			if (cur->right == NULL)
			{
				cur->right = trie_new_node();
				if (cur->right == NULL)
					return (0);
			}
			cur = cur->right;
		}
	}
	cur->is_leaf = 1;
	cur->byte = byte_val;
	return (1);
}

/**
 * read_header - Reads the magic, sizes and per-symbol code table
 * @in: File to read from
 * @original_size: Where to store the original (uncompressed) size
 * @byte_val: Where to store the sole byte value when count == 1
 * @count: Where to store the number of distinct byte values
 * @trie_root: Where to store the built decode trie (NULL if count < 2)
 *
 * Return: 1 on success, 0 on failure (including a bad magic number)
 */
static int read_header(FILE *in, unsigned long *original_size,
	unsigned char *byte_val, unsigned int *count, trienode_t **trie_root)
{
	char magic[HUFFMAN_MAGIC_LEN];
	unsigned int i;

	*trie_root = NULL;

	if (fread(magic, 1, HUFFMAN_MAGIC_LEN, in) != HUFFMAN_MAGIC_LEN)
		return (0);
	if (memcmp(magic, HUFFMAN_MAGIC, HUFFMAN_MAGIC_LEN) != 0)
		return (0);
	if (!read_u32(in, original_size) || !read_u16(in, count))
		return (0);

	if (*count == 0)
		return (1);

	if (*count == 1)
	{
		int b = fgetc(in);
		int length = fgetc(in);

		if (b == EOF || length != 0)
			return (0);
		*byte_val = (unsigned char)b;
		return (1);
	}

	*trie_root = trie_new_node();
	if (*trie_root == NULL)
		return (0);

	for (i = 0; i < *count; i++)
	{
		int b, length, nbytes;
		unsigned char bits[32];

		b = fgetc(in);
		length = fgetc(in);
		if (b == EOF || length <= 0 || length > 256)
			return (0);

		nbytes = (length + 7) / 8;
		if (fread(bits, 1, (size_t)nbytes, in) != (size_t)nbytes)
			return (0);

		if (!trie_insert(*trie_root, bits, length, (unsigned char)b))
			return (0);
	}
	return (1);
}

/**
 * decompress_file - Decompresses in into out
 * @in: Already-open input file (binary mode)
 * @out: Already-open, empty output file (binary mode)
 *
 * Return: 1 on success, 0 on failure
 */
int decompress_file(FILE *in, FILE *out)
{
	unsigned long original_size, produced;
	unsigned char single_byte;
	unsigned int count;
	trienode_t *root;
	bitreader_t br;
	trienode_t *cur;
	int bit;

	if (!read_header(in, &original_size, &single_byte, &count, &root))
	{
		free_trie(root);
		return (0);
	}

	if (original_size == 0)
		return (1);

	if (count == 1)
	{
		for (produced = 0; produced < original_size; produced++)
			if (fputc(single_byte, out) == EOF)
				return (0);
		return (1);
	}

	br_init(&br, in);
	cur = root;
	produced = 0;
	while (produced < original_size)
	{
		bit = br_get_bit(&br);
		if (bit == -1)
		{
			free_trie(root);
			return (0);
		}
		cur = bit ? cur->right : cur->left;
		if (cur == NULL)
		{
			free_trie(root);
			return (0);
		}
		if (cur->is_leaf)
		{
			if (fputc(cur->byte, out) == EOF)
			{
				free_trie(root);
				return (0);
			}
			produced++;
			cur = root;
		}
	}

	free_trie(root);
	return (1);
}
