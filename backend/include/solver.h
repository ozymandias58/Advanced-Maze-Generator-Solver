#ifndef SOLVER_H
#define SOLVER_H

#include "MazeGenerator.h"

typedef struct {
    int x;
    int y;
} Coordinate;


// Çözüm yolu (Karakterin BFS de yürüyeceği yol)
extern Coordinate BFSresult[1000];
extern int BFSresult_size;
// Çözüm yolu (Karakterin DFS de yürüyeceği yol)
extern Coordinate DFSresult[1000];
extern int DFSresult_size;
// Çözüm yolu (Karakterin Dijkstra da yürüyeceği yol)
extern Coordinate DJKresult[1000];
extern int DJKresult_size;

// BFS Arama sırası (GUI da renklendirilcek tüm hücreler)
extern Coordinate BFSexplored[1000];
extern int BFSexplored_size;
// DFS Arama sırası (GUI da renklendirilcek tüm hücreler)
extern Coordinate DFSexplored[1000];
extern int DFSexplored_size;
// Dijkstra Arama sırası (GUI da renklendirilcek tüm hücreler)
extern Coordinate DJKexplored[1000];
extern int DJKexplored_size;



void solve_BFS(int start, int end, int rows, int columns, int** AdjMatrix);

void solve_DFS(int start, int end, int rows, int columns, int** AdjMatrix);

void solve_Dijkstra(int start, int end, int rows, int collumns, int** AdjMatrix);

#endif