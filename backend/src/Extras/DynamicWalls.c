#include "MazeGenerator.h"

#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void DynamicSTKpush(int *stk, int v, int *top) {
    stk[++(*top)] = v;
}

int DynamicSTKpop(int *stk, int *top) {
    return stk[(*top)--];
}

int DynamicWallDFS(int totalCells, int adjMat[totalCells][totalCells], int src, int target) {
    //find 1 possible new connection (that node is v)
    //find a path from v to u through dfs (last is n)
    //sever the n-u
    //connect v-u

    int* stk = (int*)malloc(sizeof(int) * totalCells);
    int* visited = (int*)calloc(totalCells, sizeof(int)); // calloc initializes to 0
    int top = -1;

    DynamicSTKpush(stk, src, &top);
    visited[src] = 1;

    while(top > -1) {
        int cur = DynamicSTKpop(stk, &top);
        
        for(int i = 0; i < totalCells; i++) {
            if(adjMat[cur][i] == 1 && visited[i] == 0) {
                if(i == target) {
                    free(stk);
                    free(visited);
                    return cur; // Return n
                }
                visited[i] = 1;
                DynamicSTKpush(stk, i, &top);
            }
        }
    }
    free(stk);
    free(visited);
    return -1;
}

void DynamicWallChange(int rows, int cols, int totalCells, int adjMat[totalCells][totalCells]) {
    int u = -1, v = -1;
    
    // find a potential v
    while(u == -1) {
        int r = rand() % rows;
        int c = rand() % cols;
        u = c + (r * cols);

        int dir = rand() % 4;
        int nr = r, nc = c;
        if (dir == 0) nr--;      // Top
        else if (dir == 1) nr++; // Bottom
        else if (dir == 2) nc--; // Left
        else if (dir == 3) nc++; // Right

        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
            v = nc + (nr * cols);
            if (adjMat[u][v] == 0) {
                break;
            }
        }
        u = -1;
    }

    // find path from v to u to find n
    int n = DynamicWallDFS(totalCells, adjMat, v, u);

    if (n != -1) {
        //sever n-u and connect v-u
        adjMat[u][n] = adjMat[n][u] = 0;
        adjMat[u][v] = adjMat[v][u] = 1;
    }
}
