#include <stdio.h>
#include <time.h>
#include "MazeGenerator.h"
#include "dsu.h"
#include "solver.h"
#include "FileManager.h"
#include "testResult.h"
#include "multi.h" 

/**
 * Validates the final path found by the algorithm.
 * Returns 1 if valid, 0 if it goes through a wall or is disconnected.
 */
int validatePath(TestResult res, int** AdjMatrix, int totalCells) {
    if (res.resultCount <= 0) {
        printf("[%s] Error: No path found in result.\n", res.algo);
        return 0;
    }

    for (int i = 0; i < res.resultCount - 1; i++) {
        int u = res.result[i];
        int v = res.result[i + 1];

        // Check bounds
        if (u < 0 || u >= totalCells || v < 0 || v >= totalCells) {
            printf("[%s] Error: Result contains invalid vertex ID (%d or %d).\n", res.algo, u, v);
            return 0;
        }

        // Check if edge exists in the current state of AdjMatrix
        if (AdjMatrix[u][v] <= 0) {
            printf("[%s] FAIL: Path walks through wall between %d and %d!\n", res.algo, u, v);
            return 0;
        }
    }

    printf("[%s] SUCCESS: Path is valid and physically connected.\n", res.algo);
    return 1;
}

/**
 * Prints a summary of the TestResult for debugging.
 */
void debugTestResult(TestResult res, int** AdjMatrix, int totalCells) {
    printf("\n--- Debugging Algorithm: %s ---\n", res.algo);
    printf("Steps: %d, Weight: %d\n", res.steps, res.weight);
    printf("Explored Nodes: %d\n", res.exploredCount);
    printf("Dynamic Changes Triggered: %d\n", res.dynamicChangeCount);
    
    validatePath(res, AdjMatrix, totalCells);
    printf("----------------------------------\n");
}

int main(){
    srand(time(NULL)); // Ensure different maze changes each run
    int rows = 20, cols = 20;
    int totalCells = rows * cols;
    int iterations = 100; // Run 100 times to catch the bug
    int failCount = 0;

    for (int i = 0; i < iterations; i++) {
        edge* wallList = NULL; edge* edgeList = NULL; int edgeCount;
        
        // 1. Generate Maze
        initWalls(&wallList, rows, cols, 0);
        kruskalAlgo(&edgeList, wallList, rows, cols, &edgeCount);
        int** adjMat = convertToAdjMatrix(rows, cols, edgeList, edgeCount);
        
        // 2. Setup Solver
        dynamicMode = 1;
        int startNode = 0;
        int endNode = totalCells - 1; // End at the bottom-right

        // 3. Solve
        TestResult res = solve_Astar(startNode, endNode, rows, cols, adjMat);

        // 4. Validate
        printf("Test Run #%d: ", i + 1);
        if (!validatePath(res, adjMat, totalCells)) {
            failCount++;
            // Optional: Print details of the failure
            // debugTestResult(res, adjMat, totalCells);
        }

        // 5. Cleanup for next run
        for (int j = 0; j < totalCells; j++) free(adjMat[j]);
        free(adjMat);
        free(wallList);
        free(edgeList);
        // Be sure to free res.result, res.explored etc. to avoid memory leaks
        free(res.result); free(res.explored);
        free(res.dynamicChangeUpdates); free(res.dynamicChangeIndexes);
    }

    printf("\n=== FINAL REPORT ===\n");
    printf("Total Tests: %d\n", iterations);
    printf("Failures: %d\n", failCount);
    printf("Success Rate: %.2f%%\n", ((float)(iterations - failCount) / iterations) * 100);

    return (failCount > 0) ? 1 : 0;
}