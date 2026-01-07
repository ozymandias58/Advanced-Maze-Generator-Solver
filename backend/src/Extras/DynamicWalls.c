#include "MazeGenerator.h"
#include "DynamicWalls.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void printUpdateToJava(MatrixUpdate up) {
    // java tarafı (yani gui) "DYNA: u v w" seklinde alıyor ve REMOVE ise w zaten 0 gelecek o yüzden sıkıntı yok
    printf("DYNA: %d %d %d\n", up.u, up.v, up.w);
    fflush(stdout);
}

void DynamicSTKpush(int *stk, int v, int *top) {
    stk[++(*top)] = v;
}

int DynamicSTKpop(int *stk, int *top) {
    return stk[(*top)--];
}

int isEdgeProtected(int u, int n, int v, int* parent, int current, int* visited, int totalCells) {
    //maviyse duvar oluşmasın
    if (visited[u] || visited[n]) {
        return 1; 
    }

    //maviyse yol oluşmasın
    if (visited[v]) {
        return 1;
    }

    return 0;
}

int DynamicWallDFS(int totalCells, int** adjMat, int src, int target) {

    int* stk = (int*)malloc(sizeof(int) * totalCells);
    int* visited = (int*)calloc(totalCells, sizeof(int)); // calloc initializes to 0
    int top = -1;

    DynamicSTKpush(stk, src, &top);
    visited[src] = 1;

    while(top > -1) {
        int cur = DynamicSTKpop(stk, &top);
        
        for(int i = 0; i < totalCells; i++) {
            if(adjMat[cur][i]>0 && visited[i] == 0) {
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

MatrixUpdate* initDynamicModule(int *updateIndex){
    MatrixUpdate* updates = (MatrixUpdate*)safe_malloc(200*sizeof(MatrixUpdate));
    *updateIndex = 0;
    return updates;
}

int DynamicWallChange(int rows, int cols, int totalCells, int **adjMat, MatrixUpdate* updates, int *updateIndex, int *parent, int current, int *visited) {
    int u = -1, v = -1, attempt = 0;
    
    while(u == -1 && attempt < 100) {
        attempt++;
        int r = rand() % rows;
        int c = rand() % cols;
        u = c + (r * cols);

        int dir = rand() % 4;
        int nr = r, nc = c;
        if (dir == 0) nr--; else if (dir == 1) nr++; 
        else if (dir == 2) nc--; else if (dir == 3) nc++;

        if (nr >= 0 && nr < rows && nc >= 0 && nc < cols) {
            v = nc + (nr * cols);
            // Only pick a v that hasn't been visited by the solver yet!
            if (adjMat[u][v] == 0 && !visited[v]) {
                break;
            }
        }
        u = -1;
    }

    int n = DynamicWallDFS(totalCells, adjMat, v, u);

    if (n != -1 && (*updateIndex) < 198) {
        if (isEdgeProtected(u, n, v, parent, current, visited, totalCells)) {
            return -1; 
        }

        if (visited[n]) return -1;

        // Perform change...
        int temp_w = adjMat[u][n];
        if(temp_w == 0) temp_w = 1;
        adjMat[u][n] = adjMat[n][u] = 0;
        adjMat[u][v] = adjMat[v][u] = temp_w;
        
        updates[(*updateIndex)++] = (MatrixUpdate){UPDATE_ADD, u, v, temp_w};
        updates[(*updateIndex)++] = (MatrixUpdate){UPDATE_REMOVE, u, n, 0};
        
        printUpdateToJava(updates[(*updateIndex)-1]);
        printUpdateToJava(updates[(*updateIndex)-2]);
    }
    return u;
}