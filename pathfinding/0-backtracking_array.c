#include "pathfinding.h"

/**
 * push_point - push a new point at the front of the queue
 * @q: queue
 * @x: x coordinate
 * @y: y coordinate
 *
 * Return: 1 on success, 0 on failure
 */
static int push_point(queue_t *q, int x, int y)
{
	point_t *p = malloc(sizeof(*p));

	if (!p)
		return (0);
	p->x = x;
	p->y = y;
	if (!queue_push_front(q, p))
	{
		free(p);
		return (0);
	}
	return (1);
}

/**
 * bt - recursive backtracking step
 * @map: map
 * @d: rows (d[0]) and cols (d[1])
 * @pos: current cell (x, y)
 * @t: target
 * @seen: visited flags
 * @q: path queue
 *
 * Return: 1 if a path was found from this cell, 0 otherwise
 */
static int bt(char **map, int d[2], int pos[2], point_t const *t,
		char *seen, queue_t *q)
{
	int x = pos[0], y = pos[1], found;
	int nx[4], ny[4], i;

	if (x < 0 || y < 0 || x >= d[1] || y >= d[0] ||
	    map[y][x] == '1' || seen[y * d[1] + x])
		return (0);
	seen[y * d[1] + x] = 1;
	printf("Checking coordinates [%d, %d]\n", x, y);
	found = (x == t->x && y == t->y);
	nx[0] = x + 1, ny[0] = y;
	nx[1] = x, ny[1] = y + 1;
	nx[2] = x - 1, ny[2] = y;
	nx[3] = x, ny[3] = y - 1;
	for (i = 0; i < 4 && !found; i++)
	{
		int np[2];

		np[0] = nx[i], np[1] = ny[i];
		found = bt(map, d, np, t, seen, q);
	}
	if (found && !push_point(q, x, y))
		return (0);
	return (found);
}

/**
 * backtracking_array - first path from start to target, via backtracking
 * @map: read-only 2D array (0 walkable, 1 blocked)
 * @rows: number of rows
 * @cols: number of columns
 * @start: start point
 * @target: target point
 *
 * Return: queue of point_t forming the path, or NULL
 */
queue_t *backtracking_array(char **map, int rows, int cols,
		point_t const *start, point_t const *target)
{
	queue_t *q;
	char *seen;
	int d[2], pos[2];

	if (!map || !start || !target || rows <= 0 || cols <= 0)
		return (NULL);
	seen = calloc(rows * cols, 1);
	q = queue_create();
	if (!seen || !q)
	{
		free(seen);
		free(q);
		return (NULL);
	}
	d[0] = rows, d[1] = cols;
	pos[0] = start->x, pos[1] = start->y;
	if (!bt(map, d, pos, target, seen, q))
	{
		while (q->front)
			free(dequeue(q));
		free(q);
		q = NULL;
	}
	free(seen);
	return (q);
}
