#include "pathfinding.h"

/**
 * pick_min - unvisited vertex with the smallest finite distance
 * @vs: vertices indexed by vertex index
 * @n: number of vertices
 * @dist: distances
 * @done: visited flags
 *
 * Return: vertex, or NULL if none reachable remain
 */
static vertex_t *pick_min(vertex_t **vs, size_t n, size_t *dist, char *done)
{
	vertex_t *best = NULL;
	size_t i;

	for (i = 0; i < n; i++)
		if (!done[i] && dist[i] != (size_t)-1 &&
		    (!best || dist[i] < dist[best->index]))
			best = vs[i];
	return (best);
}

/**
 * relax - update neighbours of u
 * @u: vertex just visited
 * @dist: distances
 * @prev: predecessors
 * @done: visited flags
 */
static void relax(vertex_t *u, size_t *dist, vertex_t **prev, char *done)
{
	edge_t *e;

	for (e = u->edges; e; e = e->next)
		if (!done[e->dest->index] &&
		    dist[u->index] + e->weight < dist[e->dest->index])
		{
			dist[e->dest->index] = dist[u->index] + e->weight;
			prev[e->dest->index] = u;
		}
}

/**
 * build_path - walk predecessors back from target into a queue
 * @prev: predecessors
 * @t: target vertex
 *
 * Return: queue of char *, or NULL
 */
static queue_t *build_path(vertex_t **prev, vertex_t *t)
{
	queue_t *q = queue_create();
	char *name;

	if (!q)
		return (NULL);
	for (; t; t = prev[t->index])
	{
		name = strdup(t->content);
		if (!name || !queue_push_front(q, name))
		{
			free(name);
			while (q->front)
				free(dequeue(q));
			free(q);
			return (NULL);
		}
	}
	return (q);
}

/**
 * dijkstra_graph - shortest path using Dijkstra's algorithm
 * @graph: graph
 * @start: start vertex
 * @target: target vertex
 *
 * Return: queue of char * forming the path, or NULL
 */
queue_t *dijkstra_graph(graph_t *graph, vertex_t const *start,
		vertex_t const *target)
{
	size_t n, i, *dist;
	vertex_t **vs, **prev, *u;
	char *done;
	queue_t *q = NULL;

	if (!graph || !start || !target || !graph->nb_vertices)
		return (NULL);
	n = graph->nb_vertices;
	vs = calloc(n, sizeof(*vs));
	prev = calloc(n, sizeof(*prev));
	dist = malloc(n * sizeof(*dist));
	done = calloc(n, 1);
	if (vs && prev && dist && done)
	{
		for (u = graph->vertices; u; u = u->next)
			vs[u->index] = u;
		for (i = 0; i < n; i++)
			dist[i] = (size_t)-1;
		dist[start->index] = 0;
		while ((u = pick_min(vs, n, dist, done)))
		{
			done[u->index] = 1;
			printf("Checking %s, distance from %s is %lu\n",
			       u->content, start->content,
			       (unsigned long)dist[u->index]);
			if (u->index == target->index)
			{
				q = build_path(prev, u);
				break;
			}
			relax(u, dist, prev, done);
		}
	}
	free(vs), free(prev), free(dist), free(done);
	return (q);
}
