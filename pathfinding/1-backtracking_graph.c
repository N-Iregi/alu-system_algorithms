#include "pathfinding.h"

/**
 * bt_graph - recursive backtracking step
 * @v: current vertex
 * @target: target vertex
 * @seen: visited flags indexed by vertex index
 * @q: path queue
 *
 * Return: 1 if path found from v, 0 otherwise
 */
static int bt_graph(vertex_t const *v, vertex_t const *target,
		char *seen, queue_t *q)
{
	edge_t *e;
	int found;
	char *name;

	if (seen[v->index])
		return (0);
	seen[v->index] = 1;
	printf("Checking %s\n", v->content);
	found = (v->index == target->index);
	for (e = v->edges; e && !found; e = e->next)
		found = bt_graph(e->dest, target, seen, q);
	if (!found)
		return (0);
	name = strdup(v->content);
	if (!name || !queue_push_front(q, name))
	{
		free(name);
		return (0);
	}
	return (1);
}

/**
 * backtracking_graph - first path from start to target, via backtracking
 * @graph: graph
 * @start: start vertex
 * @target: target vertex
 *
 * Return: queue of char * forming the path, or NULL
 */
queue_t *backtracking_graph(graph_t *graph, vertex_t const *start,
		vertex_t const *target)
{
	queue_t *q;
	char *seen;

	if (!graph || !start || !target || !graph->nb_vertices)
		return (NULL);
	seen = calloc(graph->nb_vertices, 1);
	q = queue_create();
	if (!seen || !q)
	{
		free(seen);
		free(q);
		return (NULL);
	}
	if (!bt_graph(start, target, seen, q))
	{
		while (q->front)
			free(dequeue(q));
		free(q);
		q = NULL;
	}
	free(seen);
	return (q);
}
