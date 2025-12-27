#ifndef SOLVER_H
#define SOLVER_H

#include "MazeGenerator.h"

typedef struct {
    int x;
    int y;
} Coordinate;

void* safe_malloc(size_t size); //her mallocta kontrol yapmaktan sıkıldım

// Çözüm yolu (Karakterin BFS de yürüyeceği yol)
extern Coordinate BFSresult[90000];
extern int BFSresult_size;
// Çözüm yolu (Karakterin DFS de yürüyeceği yol)
extern Coordinate DFSresult[90000];
extern int DFSresult_size;
// Çözüm yolu (Karakterin Dijkstra da yürüyeceği yol)
extern Coordinate DJKresult[90000];
extern int DJKresult_size;

// BFS Arama sırası (GUI da renklendirilcek tüm hücreler)
extern Coordinate BFSexplored[90000];
extern int BFSexplored_size;
// DFS Arama sırası (GUI da renklendirilcek tüm hücreler)
extern Coordinate DFSexplored[90000];
extern int DFSexplored_size;
// Dijkstra Arama sırası (GUI da renklendirilcek tüm hücreler)
extern Coordinate DJKexplored[90000];
extern int DJKexplored_size;


int** convertToAdjMatrix(int rows, int columns, edge* edgeList, int edgeCount);

void solve_BFS(int start, int end, int rows, int columns, int** AdjMatrix);

void solve_DFS(int start, int end, int rows, int columns, int** AdjMatrix);

void solve_Dijkstra(int start, int end, int rows, int collumns, int** AdjMatrix);

#endif