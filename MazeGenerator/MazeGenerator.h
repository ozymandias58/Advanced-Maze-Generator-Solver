#ifndef MAZE_GENERATOR_H
#define MAZE_GENERATOR_H

typedef struct edge{
    int u;
    int v;
    int w;
}edge;

void initWalls(edge **wallList, int rows, int cols);
void kruskalAlgo(edge **edgeList, edge *wallList, int rows, int cols);
void printEdges(edge *edgeList, int size);

#endif