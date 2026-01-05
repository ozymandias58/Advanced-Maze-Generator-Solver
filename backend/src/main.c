#include "MazeGenerator.h"
#include "dsu.h"
#include "solver.h"
#include "FileManager.h"
#include "testResult.h"
#include "multi.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void printResultForJava(TestResult res) {
    printf("ALGO: %s\n", res.algo);
    printf("STEPS: %d\n", res.steps);
    printf("WEIGHT: %d\n", res.weight);

    if (res.dynamicChangeCount > 0 && res.dynamicChangeUpdates != NULL) {
        for (int i = 0; i < res.dynamicChangeCount; i++) {
            MatrixUpdate update = res.dynamicChangeUpdates[i];
            int finalW = (update.update == UPDATE_ADD) ? update.w : 0; 
            printf("DYNA: %d %d %d\n", update.u, update.v, finalW);
        }
    }
    printf("EXPLORED:");
    for (int i = 0; i < res.exploredCount; i++) printf(" %d", res.explored[i]);
    printf("\n");
    printf("RESULT:");
    for (int i = 0; i < res.resultCount; i++) printf(" %d", res.result[i]);
    printf("\n");
    fflush(stdout);
}

int main(int argc, char *argv[]) {
    // DEBUG: Başladığını bildir (stderr Java'da loga düşer)
    fprintf(stderr, "DEBUG: C Programi Basladi. Input Bekleniyor...\n");
    fflush(stderr);

    char* algoType = "BFS"; 
    if (argc >= 2) algoType = argv[1];
    int rows, cols, opt_weight, opt_dynamic, opt_multi;
    
    // Girdileri al
    int readCount = scanf("%d %d %d %d %d", &rows, &cols, &opt_weight, &opt_dynamic, &opt_multi);
    if (readCount != 5) {
        fprintf(stderr, "DEBUG: HATA! Input okunamadi. Okunan veri sayisi: %d\n", readCount);
        return 1; 
    }

    fprintf(stderr, "DEBUG: Input Alindi. Harita Olusturuluyor...\n");
    fflush(stderr);

    dynamicMode = opt_dynamic; 
    srand(time(0));

    edge* wallList = NULL; edge* edgeList = NULL; int edgeCount;
    initWalls(&wallList, rows, cols, opt_weight);
    kruskalAlgo(&edgeList, wallList, rows, cols, &edgeCount);
    int** adjMat = convertToAdjMatrix(rows, cols, edgeList, edgeCount);

    fprintf(stderr, "DEBUG: Harita Hazir. Matris Basiliyor...\n");
    fflush(stderr);

    int totalCells = rows * cols;
    for (int i = 0; i < totalCells; i++) {
        printf("MATRIX:"); 
        for (int j = 0; j < totalCells; j++) printf(" %d", adjMat[i][j]); 
        printf("\n");
    }
    printf("MAP_READY\n"); 
    fflush(stdout); 

    TestResult res;
    int start = 0; int end = totalCells - 1;   

    if (opt_multi) {
        int d1 = cols - 1; int d2 = (rows - 1) * cols; int d3 = end; 
        int algoID = 0; 
        if (strcmp(algoType, "DFS") == 0) algoID = 1;
        else if (strcmp(algoType, "Dijkstra") == 0) algoID = 2;
        else if (strcmp(algoType, "A*") == 0) algoID = 3;
        res = solve_MultiDest(start, d1, d2, d3, rows, cols, adjMat, algoID);
        res.algo = algoType;
    } else {
        if (strcmp(algoType, "DFS") == 0) res = solve_DFS(start, end, rows, cols, adjMat);
        else if (strcmp(algoType, "Dijkstra") == 0) res = solve_Dijkstra(start, end, rows, cols, adjMat);
        else if (strcmp(algoType, "A*") == 0) res = solve_Astar(start, end, rows, cols, adjMat);
        else res = solve_BFS(start, end, rows, cols, adjMat);
    }
    printResultForJava(res);
    return 0;
}