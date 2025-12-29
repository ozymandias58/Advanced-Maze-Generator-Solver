#include "MazeGenerator.h"
#include "dsu.h"
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

void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        printf("Malloc ile yer ayirma hatasi");
        exit(1);
    }
    return ptr;
}
void* safe_realloc(void* ptr, size_t size) {
    void* temporaryPointer = realloc(ptr, size);
    if (!temporaryPointer) {
        printf("Realloc ile yer ayirma hatasi");
        exit(1); 
    }
    return temporaryPointer;
}

int** convertToAdjMatrix(int rows,int collumns,edge* edgeList, int edgeCount){
    int totalCellCount = rows * collumns;
    int i,j;
    int** matrix = (int**)safe_malloc(totalCellCount * sizeof(int*));
    for (i = 0;i < totalCellCount;i++) {
        // satir icin yer ayir
        matrix[i] = (int*)safe_malloc(totalCellCount * sizeof(int)); 
        // satirin her bir hucresini sıfır yap
        for (j = 0;j < totalCellCount;j++) {
            matrix[i][j] = 0;
        }
    }
    //yollari giriyorum 
    for (i = 0; i < edgeCount; i++) {
        int u = edgeList[i].u;
        int v = edgeList[i].v;
        int w = edgeList[i].w;
        
        matrix[u][v] = w; 
        matrix[v][u] = w;
    }

    return matrix;
}

int* weightGenerator(int totalEdgeCount){
    int* edgeWeights = safe_malloc(totalEdgeCount * sizeof(int));
    for(int i=0;i<totalEdgeCount;i++){
        int roll = rand()%3;
        if(roll == 0) edgeWeights[i] = 2;
        else edgeWeights[i] = 1;
    }
    return edgeWeights;
}

void initWalls(edge **wallList, int rows, int cols, int option_weight){
    int wallCount = (rows * (cols - 1)) + (cols * (rows - 1));

    *wallList = (edge*)safe_malloc(sizeof(edge)*wallCount);

    //weight option
    int* edgeWeights;
    if(option_weight)
        edgeWeights = weightGenerator(wallCount);

    int idx = 0;
    for(int r=0; r<rows; r++){
        for(int c=0; c<cols; c++){
            int cur_cell = c + (r * cols);

            if(c < cols-1){ //is there a right?
                edge wall;
                wall.u = cur_cell;
                wall.v = cur_cell + 1;
                if(option_weight) wall.w = edgeWeights[idx];
                else wall.w = 1;
                (*wallList)[idx++] = wall;
            }

            if(r < rows-1){ //is there a down?
                edge wall;
                wall.u = cur_cell;
                wall.v = cur_cell + cols;
                if(option_weight) wall.w = edgeWeights[idx];
                else wall.w = 1;
                (*wallList)[idx++] = wall;
            }
        }
    }
    free(edgeWeights);
}

//Knocks walls one by one until each vertice is connected
void kruskalAlgo(edge **edgeList,edge *wallList, int rows, int cols, int* edgeCount){
    int wallCount = (rows * (cols - 1)) + (cols * (rows - 1));
    int verticeCount = rows*cols;
    edge *maxEdgeList;
    maxEdgeList = (edge*)safe_malloc(sizeof(edge)*wallCount); //max size
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
    *edgeCount = newSize;
    *edgeList = safe_realloc(maxEdgeList,newSize * sizeof(edge));
    //printEdges(*edgeList,newSize);

    free(d.parent);
    free(d.rank);
}

void printEdges(edge *edgeList, int size){
    printf("\n");
    for(int i=0;i<size;i++){
        printf("%d connects to %d with weight %d\n", edgeList[i].u,edgeList[i].v,edgeList[i].w);
    }
}

void printAdjMatrix(int** mat, int row, int col){
    int totalCellCount = row*col;
    for(int i=0;i<totalCellCount;i++){
        printf("\n%d: ",i);
        for(int j=0; j<totalCellCount; j++){
            printf("%d, ",mat[i][j]);
        }
    }
}