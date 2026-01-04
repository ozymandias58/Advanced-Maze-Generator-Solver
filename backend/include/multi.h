#ifndef MULTI_H
#define MULTI_H
#include <stdio.h>
#include <stdlib.h>
#include "solver.h"
#include "MazeGenerator.h"

int findMax(int a,int b);//returns the maximum value
TestResult mergeFunction(TestResult res1,TestResult res2,TestResult res3);//merges testresults into one final result for gui
TestResult solve_MultiDest(int start,int dest1,int dest2,int dest3,int rows,int collumns,int** AdjMatrix,int algorithmType);//0:BFS 1:DFS 2:DJK 3:A* solves multi destination problem

#endif