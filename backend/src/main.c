#include "MazeGenerator.h"
#include "dsu.h"
#include "solver.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
    srand(time(0));
    edge* edgeList = NULL;
    edge* wallList = NULL;
    int rows,cols, edgeCount;
    int** adjMat = NULL;
    int action = 0;
    int exit = 0;
    while(exit == 0){
        printf("Ana Ekran\n");
        printf("Test olusturmak icin 1\nTest yuklemek icin 2\nCikmak icin 3\n");
        scanf("%d",&action);
        if(action == 3) exit = 1;
        if(action == 2){
            printf("\n\naynen kanzi\n\n");
            continue;}
        if(action == 1){
            rows = 10;
            cols = 10;
            if(adjMat != NULL){
                for(int i=0; i<rows*cols;i++){
                    if(adjMat[i] != NULL) free(adjMat[i]);
                }
                free(adjMat);
                if(wallList != NULL) { free(wallList); wallList = NULL; }
                if(edgeList != NULL) { free(edgeList); edgeList = NULL; }
            }
            printf("before init");
            initWalls(&wallList,rows,cols);
            kruskalAlgo(&edgeList,wallList,rows,cols,&edgeCount,0);
            adjMat = convertToAdjMatrix(rows,cols,edgeList,edgeCount);
            printf("\n\n\nSecenekleri atliyorum\n\n\n");
            printf("Labirent:");
            printAdjMatrix(adjMat,rows,cols);
            exit = 1;
        }
    }


    return 1;
}