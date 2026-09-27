#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman_file.h"

/**
 * usage_error - Prints the usage message
 */
static void usage_error(void)
{
	fprintf(stderr, "Usage: huffman <mode> <filename> <out_filename>\n");
}

/**
 * main - Entry point for the huffman compression/decompression tool
 * @argc: Argument count
 * @argv: Argument vector: mode, input filename, output filename
 *
 * Return: EXIT_SUCCESS on success, EXIT_FAILURE otherwise
 */
int main(int argc, char *argv[])
{
	FILE *in, *out, *existing;
	int ok;

	if (argc != 4 || strlen(argv[1]) != 1 ||
		(argv[1][0] != 'c' && argv[1][0] != 'd'))
	{
		usage_error();
		return (EXIT_FAILURE);
	}

	in = fopen(argv[2], "rb");
	if (in == NULL)
	{
		fprintf(stderr, "No such file: %s\n", argv[2]);
		return (EXIT_FAILURE);
	}

	existing = fopen(argv[3], "rb");
	if (existing != NULL)
	{
		fclose(existing);
		fclose(in);
		fprintf(stderr, "File already exists: %s\n", argv[3]);
		return (EXIT_FAILURE);
	}

	out = fopen(argv[3], "wb");
	if (out == NULL)
	{
		fprintf(stderr, "Could not create file: %s\n", argv[3]);
		fclose(in);
		return (EXIT_FAILURE);
	}

	if (argv[1][0] == 'c')
		ok = compress_file(in, out);
	else
		ok = decompress_file(in, out);

	fclose(in);
	fclose(out);

	if (!ok)
	{
		fprintf(stderr, "Failed to process file: %s\n", argv[2]);
		remove(argv[3]);
		return (EXIT_FAILURE);
	}

	return (EXIT_SUCCESS);
}
