#ifndef SOLVER_H
#define SOLVER_H

#include "MazeGenerator.h"
#include "testresult.h"

#define DEFAULTSIZE 1000
#define INF 999999 //sonsuzluga yakin yani
#define COST 90000

typedef struct {
    int x;
    int y;
} Coordinate;


// Çözüm yolu (Karakterin BFS de yürüyeceği yol)
extern Coordinate BFSresult[DEFAULTSIZE];
extern int BFSresult_size;
// Çözüm yolu (Karakterin DFS de yürüyeceği yol)
extern Coordinate DFSresult[DEFAULTSIZE];
extern int DFSresult_size;
// Çözüm yolu (Karakterin Dijkstra da yürüyeceği yol)
extern Coordinate DJKresult[DEFAULTSIZE];
extern int DJKresult_size;
// Çözüm yolu (Karakterin A* da yürüyeceği yol)
extern Coordinate ASTresult[DEFAULTSIZE];
extern int ASTresult_size;

// BFS Arama sırası (GUI da renklendirilcek tüm hücreler)
extern Coordinate BFSexplored[DEFAULTSIZE];
extern int BFSexplored_size;
// DFS Arama sırası (GUI da renklendirilcek tüm hücreler)
extern Coordinate DFSexplored[DEFAULTSIZE];
extern int DFSexplored_size;
// Dijkstra Arama sırası (GUI da renklendirilcek tüm hücreler)
extern Coordinate DJKexplored[DEFAULTSIZE];
extern int DJKexplored_size;
//A* Arama sırası (GUI da renklendirilcek tüm hücreler) 
extern Coordinate ASTexplored[DEFAULTSIZE];
extern int ASTexplored_size;


TestResult solve_BFS(int start, int end, int rows, int columns, int** AdjMatrix);

TestResult solve_DFS(int start, int end, int rows, int columns, int** AdjMatrix);

TestResult solve_Dijkstra(int start, int end, int rows, int collumns, int** AdjMatrix);

TestResult solve_Astar(int start, int end, int rows, int collumns, int** AdjMatrix);

#endif