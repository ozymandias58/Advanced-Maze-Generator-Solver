#include "MazeGenerator.h"
#include "..\Disjoint Set Union\dsu.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
GENERATION:
1- Create an edge list from grid, only look at right and down to not duplicate edges
2- Turn it into an mst
3- Turn that into an adjacency matrix if needed
*/

//All vertices are connected through walls.
void initWalls(edge **wallList, int rows, int cols){
    int wallCount = (rows * (cols - 1)) + (cols * (rows - 1));

    *wallList = (edge*)malloc(sizeof(edge)*wallCount);

    int idx = 0;
    for(int r=0; r<rows; r++){
        for(int c=0; c<cols; c++){
            int cur_cell = c + (r * cols);

            if(c < cols-1){ //is there a right?
                edge wall;
                wall.u = cur_cell;
                wall.v = cur_cell + 1;
                wall.w = 1;
                (*wallList)[idx++] = wall;
            }

            if(r < rows-1){ //is there a down?
                edge wall;
                wall.u = cur_cell;
                wall.v = cur_cell + cols;
                wall.w = 1;
                (*wallList)[idx++] = wall;
            }
        }
    }
}

//Knocks walls one by one until each vertice is connected
void kruskalAlgo(edge **edgeList,edge *wallList, int rows, int cols){
    int wallCount = (rows * (cols - 1)) + (cols * (rows - 1));
    int verticeCount = rows*cols;
    edge *maxEdgeList;
    maxEdgeList = (edge*)malloc(sizeof(edge)*wallCount); //max size
    if(maxEdgeList == NULL){
        printf("not enough memory\n");
        exit(1);
    }
    DSU d;

    initDSU(&d,verticeCount);
    int newSize = 0;

    //random sort
    for (int i = wallCount - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        edge temp = wallList[i];
        wallList[i] = wallList[j];
        wallList[j] = temp;
    }

    for(int i=0; i<wallCount;i++){
        int u = wallList[i].u;
        int v = wallList[i].v;
        int w = wallList[i].w;

        if(find(&d, wallList[i].u) != find(&d, wallList[i].v)){
            unionSet(&d, wallList[i].u, wallList[i].v);
            edge edge;
            edge.u = u;
            edge.v = v;
            edge.w = w;
            maxEdgeList[newSize++] = edge;
        }
    }
    *edgeList = realloc(maxEdgeList,newSize * sizeof(edge));
    printEdges(*edgeList,newSize);

    free(d.parent);
    free(d.rank);
}

void printEdges(edge *edgeList, int size){
    printf("\n");
    for(int i=0;i<size;i++){
        printf("%d connects to %d with weight %d\n", edgeList[i].u,edgeList[i].v,edgeList[i].w);
    }
}