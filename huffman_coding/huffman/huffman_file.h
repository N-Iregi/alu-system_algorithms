#ifndef HUFFMAN_FILE_H
#define HUFFMAN_FILE_H

#include <stdio.h>

/*
 * On-disk format (designed for this tool; not dictated by the spec):
 *
 *   4 bytes   magic "HUFC"
 *   4 bytes   original file size, big-endian
 *   2 bytes   number of distinct byte values in the file, big-endian
 *             (0 for an empty input file)
 *
 *   For each distinct byte value (skipped entirely if the count above
 *   is 0):
 *     1 byte  the byte value
 *     1 byte  code length in bits (0 only when there is exactly one
 *             distinct byte value in the whole file)
 *     ceil(length / 8) bytes  the code, packed MSB-first
 *
 *   If there is more than one distinct byte value, the rest of the
 *   file is the MSB-first bit-packed encoded data, in original byte
 *   order. Trailing bits in the final byte are padding and are
 *   ignored (decoding stops once original_size bytes have been
 *   produced).
 *
 *   If there is exactly one distinct byte value, no encoded data
 *   follows: decompression just writes that byte original_size
 *   times.
 */

#define HUFFMAN_MAGIC "HUFC"
#define HUFFMAN_MAGIC_LEN 4

int compress_file(FILE *in, FILE *out);
int decompress_file(FILE *in, FILE *out);

/* bitio.c */

typedef struct bitwriter_s
{
	FILE *fp;
	unsigned char buffer;
	int count;
} bitwriter_t;

typedef struct bitreader_s
{
	FILE *fp;
	unsigned char buffer;
	int count;
} bitreader_t;

void bw_init(bitwriter_t *bw, FILE *fp);
void bw_put_bit(bitwriter_t *bw, int bit);
void bw_flush(bitwriter_t *bw);

void br_init(bitreader_t *br, FILE *fp);
int br_get_bit(bitreader_t *br);

int write_u32(FILE *fp, unsigned long value);
int read_u32(FILE *fp, unsigned long *value);
int write_u16(FILE *fp, unsigned int value);
int read_u16(FILE *fp, unsigned int *value);

#endif /* HUFFMAN_FILE_H */
