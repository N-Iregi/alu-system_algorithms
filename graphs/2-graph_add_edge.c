#include "graphs.h"
#include <stdlib.h>
#include <string.h>

/**
 * find_vertex - Finds the vertex storing a given string
 * @graph: Pointer to the graph
 * @str: String to look for
 *
 * Return: Pointer to the vertex, or NULL if not found
 */
static vertex_t *find_vertex(const graph_t *graph, const char *str)
{
	vertex_t *cur;

	for (cur = graph->vertices; cur; cur = cur->next)
		if (strcmp(cur->content, str) == 0)
			return (cur);

	return (NULL);
}

/**
 * link_edge - Appends an already allocated edge to a vertex's edge list
 * @from: Vertex the edge starts from
 * @edge: Preallocated edge
 * @to: Vertex the edge points to
 */
static void link_edge(vertex_t *from, edge_t *edge, vertex_t *to)
{
	edge_t *tail;

	edge->dest = to;
	edge->next = NULL;

	if (!from->edges)
	{
		from->edges = edge;
	}
	else
	{
		tail = from->edges;
		while (tail->next)
			tail = tail->next;
		tail->next = edge;
	}

	from->nb_edges++;
}

/**
 * graph_add_edge - Adds an edge between two vertices of a graph
 * @graph: Pointer to the graph
 * @src: String identifying the source vertex
 * @dest: String identifying the destination vertex
 * @type: UNIDIRECTIONAL (src -> dest) or BIDIRECTIONAL (both ways)
 *
 * Return: 1 on success, 0 on failure (nothing is created on failure)
 */
int graph_add_edge(graph_t *graph, const char *src, const char *dest,
                   edge_type_t type)
{
	vertex_t *from, *to;
	edge_t *forward, *backward = NULL;

	if (!graph || !src || !dest)
		return (0);

	from = find_vertex(graph, src);
	to = find_vertex(graph, dest);
	if (!from || !to)
		return (0);

	forward = malloc(sizeof(edge_t));
	if (!forward)
		return (0);

	if (type == BIDIRECTIONAL)
	{
		backward = malloc(sizeof(edge_t));
		if (!backward)
		{
			free(forward);
			return (0);
		}
	}

	link_edge(from, forward, to);
	if (backward)
		link_edge(to, backward, from);

	return (1);
}
