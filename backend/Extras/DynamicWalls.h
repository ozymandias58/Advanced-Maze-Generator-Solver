#ifndef DYNAMIC_WALLS_H
#define DYNAMIC_WALLS_H

void DynamicSTKpush(int *stk, int v, int *top);
int DynamicSTKpop(int *stk, int *top);
int DynamicWallDFS(int totalCells, int (*adjMat)[*], int src, int target);
void DynamicWallChange(int rows, int cols, int totalCells, int (*adjMat)[*]);


#endif