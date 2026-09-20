#include "graphs.h"
#include <stdlib.h>
#include <string.h>

/**
 * graph_add_vertex - Adds a vertex to the end of a graph's vertex list
 * @graph: Pointer to the graph
 * @str: String to copy into the new vertex
 *
 * Return: Pointer to the new vertex, or NULL on failure
 *         (including when a vertex already stores @str)
 */
vertex_t *graph_add_vertex(graph_t *graph, const char *str)
{
	vertex_t *vertex, *tail = NULL, *cur;

	if (!graph || !str)
		return (NULL);

	for (cur = graph->vertices; cur; cur = cur->next)
	{
		if (strcmp(cur->content, str) == 0)
			return (NULL);
		tail = cur;
	}

	vertex = malloc(sizeof(vertex_t));
	if (!vertex)
		return (NULL);

	vertex->content = strdup(str);
	if (!vertex->content)
	{
		free(vertex);
		return (NULL);
	}

	vertex->index = graph->nb_vertices;
	vertex->nb_edges = 0;
	vertex->edges = NULL;
	vertex->next = NULL;

	if (tail)
		tail->next = vertex;
	else
		graph->vertices = vertex;
	graph->nb_vertices++;

	return (vertex);
}
