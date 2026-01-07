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

// Java arayüzüne sonuçları gönderen fonksiyon
void printResultForJava(TestResult res) {
    // 1. Temel Bilgiler (flush ile anında gönderim)
    printf("ALGO: %s\n", res.algo);
    printf("STEPS: %d\n", res.steps);
    printf("WEIGHT: %d\n", res.weight);

    // 2. Dinamik Duvar Değişimleri
    if (res.dynamicChangeCount > 0 && res.dynamicChangeUpdates != NULL) {
        for (int i = 0; i < res.dynamicChangeCount; i++) {
            MatrixUpdate update = res.dynamicChangeUpdates[i];
            int finalW = (update.update == UPDATE_ADD) ? update.w : 0;
            printf("DYNA: %d %d %d\n", update.u, update.v, finalW);
        }
    }

    // 3. Keşfedilen Hücreler
    printf("EXPLORED:");
    for (int i = 0; i < res.exploredCount; i++) printf(" %d", res.explored[i]);
    printf("\n");

    // 4. Sonuç Yolu
    printf("RESULT:");
    for (int i = 0; i < res.resultCount; i++) printf(" %d", res.result[i]);
    printf("\n");
    
    fflush(stdout); // Java'nın beklemesini önle
}

int main(int argc, char *argv[]) {
    // Hata mesajlarını Java konsoluna düşürmek için stderr kullanıyoruz
    fprintf(stderr, "DEBUG: C Backend Basladi.\n");
    fflush(stderr);

    char* algoType = "BFS";
    if (argc >= 2) algoType = argv[1];

    int rows, cols, opt_weight, opt_dynamic, opt_multi, mode;
    long long seed;

    // PROTOKOL: ROWS COLS WEIGHT DYNAMIC MULTI SEED MODE
    int readCount = scanf("%d %d %d %d %d %lld %d", 
                          &rows, &cols, &opt_weight, &opt_dynamic, &opt_multi, &seed, &mode);
    
    if (readCount != 7) {
        fprintf(stderr, "DEBUG: HATA! Eksik parametre. Okunan: %d\n", readCount);
        return 1; 
    }

    // SEED MANTIĞI: Java'dan gelen seed ile aynı haritayı tekrar üret
    srand((unsigned int)seed); 
    dynamicMode = opt_dynamic; 

    // HARİTA OLUŞTURMA
    edge* wallList = NULL; 
    edge* edgeList = NULL; 
    int edgeCount = 0;

    initWalls(&wallList, rows, cols, opt_weight);
    kruskalAlgo(&edgeList, wallList, rows, cols, &edgeCount);
    int** adjMat = convertToAdjMatrix(rows, cols, edgeList, edgeCount);

    // MATRİSİ JAVA'YA BAS (Satır satır)
    int totalCells = rows * cols;
    for (int i = 0; i < totalCells; i++) {
        printf("MATRIX:"); 
        for (int j = 0; j < totalCells; j++) {
            printf(" %d", adjMat[i][j]);
        }
        printf("\n");
    }
    printf("MAP_READY\n");
    fflush(stdout);

    // EĞER MOD 0 İSE (SADECE HARİTA OLUŞTUR) ÇÖZMEDEN ÇIK
    if (mode == 0) {
        // Temizlik
        for (int i = 0; i < totalCells; i++) free(adjMat[i]);
        free(adjMat);
        if(wallList) free(wallList);
        if(edgeList) free(edgeList);
        return 0;
    }

    // MOD 1 İSE ÇÖZ
    TestResult res;
    memset(&res, 0, sizeof(TestResult));

    int start = 0; 
    int end = totalCells - 1;

    if (opt_multi) {
        int d1 = cols - 1; 
        int d2 = (rows - 1) * cols; 
        int d3 = end;
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

    // SONUCU BAS (KAYDETME YOK)
    printResultForJava(res);

    // TEMİZLİK
    for (int i = 0; i < totalCells; i++) free(adjMat[i]);
    free(adjMat);
    if(wallList) free(wallList);
    if(edgeList) free(edgeList);
    if(res.result) free(res.result);
    if(res.explored) free(res.explored);
    if(res.dynamicChangeIndexes) free(res.dynamicChangeIndexes);
    if(res.dynamicChangeUpdates) free(res.dynamicChangeUpdates);

    return 0;
}