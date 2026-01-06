#ifndef DYNAMIC_WALLS_H
#define DYNAMIC_WALLS_H
enum UPDATE{
    UPDATE_ADD,
    UPDATE_REMOVE
};

typedef struct MatrixUpdate{
    enum UPDATE update;
    int u;//which cell
    int v;//target cell
    int w;//weight
}MatrixUpdate;

void DynamicSTKpush(int *stk, int v, int *top);
int DynamicSTKpop(int *stk, int *top);
int DynamicWallDFS(int totalCells, int **adjMat, int src, int target);
MatrixUpdate* initDynamicModule(int *updateIndex); //call this first. It initializes 10 updates
int DynamicWallChange(int rows, int cols, int totalCells, int **adjMat, MatrixUpdate *updates, int *updateIndex, int *parent, int current);
#endif