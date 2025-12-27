#ifndef MAZE_GENERATOR_H
#define MAZE_GENERATOR_H

typedef struct edge{
    int u;
    int v;
    int w;
}edge;

void* safe_malloc(size_t size); //her mallocta kontrol yapmaktan sıkıldım
void* safe_realloc(void* ptr, size_t size);
int** convertToAdjMatrix(int rows, int columns, edge* edgeList, int edgeCount);
void initWalls(edge **wallList, int rows, int cols);
void kruskalAlgo(edge **edgeList, edge *wallList, int rows, int cols);
void printEdges(edge *edgeList, int size);

#endif