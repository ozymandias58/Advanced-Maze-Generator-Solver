#include "multi.h"
#include "solver.h"
#include "MazeGenerator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int findMax(int a, int b) { return (a > b ? a : b); }

TestResult mergeFunction(TestResult res1, TestResult res2, TestResult res3) {
    TestResult finalRes;
    finalRes.steps = res1.steps + res2.steps + res3.steps;
    finalRes.weight = res1.weight + res2.weight + res3.weight;
    finalRes.exploredCount = res1.exploredCount + res2.exploredCount + res3.exploredCount;
    finalRes.resultCount = res1.resultCount + res2.resultCount + res3.resultCount;
    finalRes.dynamicChangeCount = res1.dynamicChangeCount + res2.dynamicChangeCount + res3.dynamicChangeCount;

    finalRes.explored = (int*)malloc(finalRes.exploredCount * sizeof(int));
    finalRes.result = (int*)malloc(finalRes.resultCount * sizeof(int));
    finalRes.dynamicChangeUpdates = (MatrixUpdate*)malloc(sizeof(MatrixUpdate) * (finalRes.dynamicChangeCount + 1));
    finalRes.dynamicChangeIndexes = (int*)malloc(sizeof(int) * (finalRes.dynamicChangeCount + 1));

    int expIdx = 0, resIdx = 0, dynIdx = 0;

    // 1. Bölüm
    for(int i=0; i<res1.exploredCount; i++) finalRes.explored[expIdx++] = res1.explored[i];
    for(int i=0; i<res1.resultCount; i++) finalRes.result[resIdx++] = res1.result[i];
    for(int i=0; i<res1.dynamicChangeCount; i++) finalRes.dynamicChangeUpdates[dynIdx++] = res1.dynamicChangeUpdates[i];

    // 2. Bölüm
    for(int i=0; i<res2.exploredCount; i++) finalRes.explored[expIdx++] = res2.explored[i];
    for(int i=0; i<res2.resultCount; i++) finalRes.result[resIdx++] = res2.result[i];
    for(int i=0; i<res2.dynamicChangeCount; i++) finalRes.dynamicChangeUpdates[dynIdx++] = res2.dynamicChangeUpdates[i];

    // 3. Bölüm
    for(int i=0; i<res3.exploredCount; i++) finalRes.explored[expIdx++] = res3.explored[i];
    for(int i=0; i<res3.resultCount; i++) finalRes.result[resIdx++] = res3.result[i];
    for(int i=0; i<res3.dynamicChangeCount; i++) finalRes.dynamicChangeUpdates[dynIdx++] = res3.dynamicChangeUpdates[i];

    return finalRes;
}

TestResult solve_MultiDest(int start, int dest1, int dest2, int dest3, int rows, int collumns, int** AdjMatrix, int algorithmType) {
    TestResult res1, res2, res3;
    // Zincirleme Rota: Start->D1, D1->D2, D2->End
    if (algorithmType == 1) { // DFS
        res1 = solve_DFS(start, dest1, rows, collumns, AdjMatrix);
        res2 = solve_DFS(dest1, dest2, rows, collumns, AdjMatrix);
        res3 = solve_DFS(dest2, dest3, rows, collumns, AdjMatrix);
    } else if (algorithmType == 2) { // Dijkstra
        res1 = solve_Dijkstra(start, dest1, rows, collumns, AdjMatrix);
        res2 = solve_Dijkstra(dest1, dest2, rows, collumns, AdjMatrix);
        res3 = solve_Dijkstra(dest2, dest3, rows, collumns, AdjMatrix);
    } else if (algorithmType == 3) { // A*
        res1 = solve_Astar(start, dest1, rows, collumns, AdjMatrix);
        res2 = solve_Astar(dest1, dest2, rows, collumns, AdjMatrix);
        res3 = solve_Astar(dest2, dest3, rows, collumns, AdjMatrix);
    } else { // BFS
        res1 = solve_BFS(start, dest1, rows, collumns, AdjMatrix);
        res2 = solve_BFS(dest1, dest2, rows, collumns, AdjMatrix);
        res3 = solve_BFS(dest2, dest3, rows, collumns, AdjMatrix);
    }
    return mergeFunction(res1, res2, res3);
}