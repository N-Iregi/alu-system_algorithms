#include "huffman_file.h"

/**
 * bw_init - Initializes a bit writer over an already-open file
 * @bw: Bit writer to initialize
 * @fp: File to write to
 */
void bw_init(bitwriter_t *bw, FILE *fp)
{
	bw->fp = fp;
	bw->buffer = 0;
	bw->count = 0;
}

/**
 * bw_put_bit - Appends a single bit (MSB-first within each byte)
 * @bw: Bit writer
 * @bit: 0 or 1
 */
void bw_put_bit(bitwriter_t *bw, int bit)
{
	bw->buffer = (unsigned char)((bw->buffer << 1) | (bit & 1));
	bw->count++;
	if (bw->count == 8)
	{
		fputc(bw->buffer, bw->fp);
		bw->buffer = 0;
		bw->count = 0;
	}
}

/**
 * bw_flush - Pads any partial final byte with zeros and writes it
 * @bw: Bit writer to flush
 */
void bw_flush(bitwriter_t *bw)
{
	if (bw->count > 0)
	{
		bw->buffer = (unsigned char)(bw->buffer << (8 - bw->count));
		fputc(bw->buffer, bw->fp);
		bw->buffer = 0;
		bw->count = 0;
	}
}

/**
 * br_init - Initializes a bit reader over an already-open file
 * @br: Bit reader to initialize
 * @fp: File to read from
 */
void br_init(bitreader_t *br, FILE *fp)
{
	br->fp = fp;
	br->buffer = 0;
	br->count = 0;
}

/**
 * br_get_bit - Reads a single bit (MSB-first within each byte)
 * @br: Bit reader
 *
 * Return: 0 or 1, or -1 on end of file
 */
int br_get_bit(bitreader_t *br)
{
	int byte;

	if (br->count == 0)
	{
		byte = fgetc(br->fp);
		if (byte == EOF)
			return (-1);
		br->buffer = (unsigned char)byte;
		br->count = 8;
	}
	br->count--;
	return ((br->buffer >> br->count) & 1);
}

/**
 * write_u32 - Writes a 4-byte big-endian unsigned integer
 * @fp: File to write to
 * @value: Value to write
 *
 * Return: 1 on success, 0 on failure
 */
int write_u32(FILE *fp, unsigned long value)
{
	unsigned char bytes[4];

	bytes[0] = (unsigned char)((value >> 24) & 0xFF);
	bytes[1] = (unsigned char)((value >> 16) & 0xFF);
	bytes[2] = (unsigned char)((value >> 8) & 0xFF);
	bytes[3] = (unsigned char)(value & 0xFF);

	return (fwrite(bytes, 1, 4, fp) == 4);
}

/**
 * read_u32 - Reads a 4-byte big-endian unsigned integer
 * @fp: File to read from
 * @value: Where to store the value read
 *
 * Return: 1 on success, 0 on failure
 */
int read_u32(FILE *fp, unsigned long *value)
{
	unsigned char bytes[4];

	if (fread(bytes, 1, 4, fp) != 4)
		return (0);

	*value = ((unsigned long)bytes[0] << 24) | ((unsigned long)bytes[1] << 16)
		| ((unsigned long)bytes[2] << 8) | (unsigned long)bytes[3];

	return (1);
}

/**
 * write_u16 - Writes a 2-byte big-endian unsigned integer
 * @fp: File to write to
 * @value: Value to write
 *
 * Return: 1 on success, 0 on failure
 */
int write_u16(FILE *fp, unsigned int value)
{
	unsigned char bytes[2];

	bytes[0] = (unsigned char)((value >> 8) & 0xFF);
	bytes[1] = (unsigned char)(value & 0xFF);

	return (fwrite(bytes, 1, 2, fp) == 2);
}

/**
 * read_u16 - Reads a 2-byte big-endian unsigned integer
 * @fp: File to read from
 * @value: Where to store the value read
 *
 * Return: 1 on success, 0 on failure
 */
int read_u16(FILE *fp, unsigned int *value)
{
	unsigned char bytes[2];

	if (fread(bytes, 1, 2, fp) != 2)
		return (0);

	*value = ((unsigned int)bytes[0] << 8) | (unsigned int)bytes[1];

	return (1);
}
