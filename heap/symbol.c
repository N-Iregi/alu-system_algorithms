#include <stdlib.h>
#include "huffman.h"

/**
 * symbol_create - Creates a symbol_t data structure
 * @data: The character to store
 * @freq: The character's associated frequency
 *
 * Return: Pointer to the created structure, or NULL on failure
 */
symbol_t *symbol_create(char data, size_t freq)
{
	symbol_t *symbol;

	symbol = malloc(sizeof(symbol_t));
	if (symbol == NULL)
		return (NULL);

	symbol->data = data;
	symbol->freq = freq;

	return (symbol);
}
