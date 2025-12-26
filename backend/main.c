#include "MazeGenerator/MazeGenerator.h"
#include "Disjoint Set Union/dsu.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
    srand(time(0));
    edge* edgeList;
    edge* wallList;
    int rows,cols;
    printf("enter row amount: ");
    scanf("%d",&rows);
    printf("\nenter column amount: ");
    scanf("%d",&cols);

    initWalls(&wallList,rows,cols);
    kruskalAlgo(&edgeList,wallList,rows,cols);

    return 1;
}