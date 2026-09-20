#include "graphs.h"
#include <stdlib.h>

/**
 * breadth_first_traverse - Traverses a graph breadth-first from its first
 * vertex
 * @graph: Pointer to the graph to traverse
 * @action: Function called on each visited vertex
 *
 * Return: The biggest vertex depth, or 0 on failure
 */
size_t breadth_first_traverse(const graph_t *graph, void (*action)(const vertex_t *v, size_t depth))
{
	const vertex_t **queue, *vertex;
	size_t *depths, head = 0, tail = 0, max = 0, depth;
	char *visited;
	edge_t *edge;

	if (!graph || !graph->vertices || !action)
		return (0);

	queue = malloc(sizeof(*queue) * graph->nb_vertices);
	depths = malloc(sizeof(size_t) * graph->nb_vertices);
	visited = calloc(graph->nb_vertices, sizeof(char));
	if (!queue || !depths || !visited)
	{
		free(queue);
		free(depths);
		free(visited);
		return (0);
	}

	visited[graph->vertices->index] = 1;
	queue[tail] = graph->vertices;
	depths[tail++] = 0;

	while (head < tail)
	{
		vertex = queue[head];
		depth = depths[head++];
		action(vertex, depth);
		if (depth > max)
			max = depth;

		for (edge = vertex->edges; edge; edge = edge->next)
		{
			if (visited[edge->dest->index])
				continue;
			visited[edge->dest->index] = 1;
			queue[tail] = edge->dest;
			depths[tail++] = depth + 1;
		}
	}
	free(queue);
	free(depths);
	free(visited);
	return (max);
}
