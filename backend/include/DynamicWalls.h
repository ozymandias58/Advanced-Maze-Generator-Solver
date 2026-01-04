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
int initDynamicModule(MatrixUpdate *updates, int *updateIndex); //call this first. It initializes 10 updates
int DynamicWallChange(int rows, int cols, int totalCells, int **adjMat, MatrixUpdate *updates, int *updateIndex); //eğer return ettiği ziyaretimize komşuysa onu da queue stack heap falan ekle

#endif