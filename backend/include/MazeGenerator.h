#ifndef MAZE_GENERATOR_H
#define MAZE_GENERATOR_H

#include <stdio.h>

typedef struct edge{ //kenar edge yani
    int u; //from
    int v;  //to
    int w;  //weight
}edge;

void* safe_malloc(size_t size); //her mallocta kontrol yapmaktan sıkıldım
void* safe_realloc(void* ptr, size_t size); //her reallocta kontrol yapmaktan sıkıldım
int** convertToAdjMatrix(int rows, int columns, edge* edgeList, int edgeCount);//edge listten adj matrixe dönüştürme
int getPerpendicularDistance(int currentID,int targetID,int collumns);
int *weightGenerator(int totalEdgeCount);//ağırlık generatörü
void initWalls(edge **wallList, int rows, int cols, int option_weight);
void kruskalAlgo(edge **edgeList,edge *wallList, int rows, int cols, int* edgeCount);
void printEdges(edge *edgeList, int size);
void printAdjMatrix(int **mat, int row, int col);

#endif