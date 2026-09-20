#include "graphs.h"
#include <stdlib.h>

/**
 * dfs_visit - Recursively visits a vertex, then its unvisited neighbours
 * @vertex: Vertex to visit
 * @visited: Array flagging visited vertices, indexed by vertex->index
 * @depth: Depth of @vertex from the starting vertex
 * @max: Pointer to the biggest depth seen so far
 * @action: Function called on each visited vertex
 */
static void dfs_visit(const vertex_t *vertex, char *visited, size_t depth,
                      size_t *max,
                      void (*action)(const vertex_t *v, size_t depth))
{
	edge_t *edge;

	visited[vertex->index] = 1;
	action(vertex, depth);
	if (depth > *max)
		*max = depth;

	for (edge = vertex->edges; edge; edge = edge->next)
		if (!visited[edge->dest->index])
			dfs_visit(edge->dest, visited, depth + 1, max, action);
}

/**
 * depth_first_traverse - Traverses a graph depth-first from its first vertex
 * @graph: Pointer to the graph to traverse
 * @action: Function called on each visited vertex
 *
 * Return: The biggest vertex depth, or 0 on failure
 */
size_t depth_first_traverse(const graph_t *graph,
                            void (*action)(const vertex_t *v, size_t depth))
{
	char *visited;
	size_t max = 0;

	if (!graph || !graph->vertices || !action)
		return (0);

	visited = calloc(graph->nb_vertices, sizeof(char));
	if (!visited)
		return (0);

	dfs_visit(graph->vertices, visited, 0, &max, action);
	free(visited);

	return (max);
}
