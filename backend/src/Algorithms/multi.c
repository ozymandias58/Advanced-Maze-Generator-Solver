#include <stdio.h>
#include <stdlib.h>
#include "solver.h"
#include "MazeGenerator.h"

int findMax(int a,int b){ return (a>b?a:b);}//returns the maximum value

TestResult mergeFunction(TestResult res1,TestResult res2,TestResult res3){
    TestResult finalRes;
    finalRes.resultCount=res1.resultCount+res2.resultCount+res3.resultCount;
    finalRes.exploredCount=res1.exploredCount+res2.exploredCount+res3.exploredCount;
    finalRes.steps=res1.steps+res2.steps+res3.steps;
    finalRes.weight=res1.weight+res2.weight+res3.weight;
    finalRes.result=(int*)safe_malloc(finalRes.resultCount*sizeof(int));
    finalRes.explored=(int*)safe_malloc(finalRes.exploredCount*sizeof(int));
    finalRes.dynamicChangeIndexes=(int*)safe_malloc(DYNAMICINDEXSIZE*sizeof(int));
    finalRes.dynamicChangeUpdates=(MatrixUpdate*)safe_malloc(DYNAMICUPDATESIZE*sizeof(MatrixUpdate));
    finalRes.dynamicChangeCount=res1.dynamicChangeCount;

    int exploredMax=findMax(res1.exploredCount,findMax(res2.exploredCount,res3.exploredCount));
    int resultMax=findMax(res1.resultCount,findMax(res2.resultCount,res3.resultCount));
    int finalExploredPosition=0,finalResultPosition=0,i;

    for(i=0;i<exploredMax;i++){
        if(i<res1.exploredCount)
            finalRes.explored[finalExploredPosition++]=res1.explored[i];        
        if(i<res2.exploredCount)
            finalRes.explored[finalExploredPosition++]=res2.explored[i];        
        if(i<res3.exploredCount)
            finalRes.explored[finalExploredPosition++]=res3.explored[i];
    }
    for(i=0;i<resultMax;i++){
        if(i<res1.resultCount)
            finalRes.result[finalResultPosition++]=res1.result[i];
        if(i<res2.resultCount)
            finalRes.result[finalResultPosition++]=res2.result[i];
        if(i<res3.resultCount)
            finalRes.result[finalResultPosition++]=res3.result[i];
    }
    for(i=0;i<res1.dynamicChangeCount;i++) {
        finalRes.dynamicChangeUpdates[i]=res1.dynamicChangeUpdates[i];
        finalRes.dynamicChangeIndexes[i]=res1.dynamicChangeIndexes[i];
    }
    return finalRes;
}

TestResult solve_MultiDest(int start,int dest1,int dest2,int dest3,int rows,int collumns,int** AdjMatrix,int algorithmType){
    TestResult res1,res2,res3;
    switch (algorithmType)
    {
    case 0:
        res1=solve_BFS(start,dest1,rows,collumns,AdjMatrix);
        res2=solve_BFS(start,dest2,rows,collumns,AdjMatrix);
        res3=solve_BFS(start,dest3,rows,collumns,AdjMatrix);
        break;
    case 1:
        res1=solve_DFS(start,dest1,rows,collumns,AdjMatrix);
        res2=solve_DFS(start,dest2,rows,collumns,AdjMatrix);
        res3=solve_DFS(start,dest3,rows,collumns,AdjMatrix);
        break;
    case 2:
        res1=solve_Dijkstra(start,dest1,rows,collumns,AdjMatrix);
        res2=solve_Dijkstra(start,dest2,rows,collumns,AdjMatrix);
        res3=solve_Dijkstra(start,dest3,rows,collumns,AdjMatrix);
        break;
    case 3:
        res1=solve_Astar(start,dest1,rows,collumns,AdjMatrix);
        res2=solve_Astar(start,dest2,rows,collumns,AdjMatrix);
        res3=solve_Astar(start,dest3,rows,collumns,AdjMatrix);
        break;
    default:
        break;
    }
    TestResult finalResult=mergeFunction(res1,res2,res3);
    finalResult.algo=res1.algo;
    free(res1.result);free(res1.explored);
    free(res1.dynamicChangeIndexes);free(res1.dynamicChangeUpdates);
    free(res2.result);free(res2.explored);
    free(res2.dynamicChangeIndexes);free(res2.dynamicChangeUpdates);
    free(res3.result);free(res3.explored);
    free(res3.dynamicChangeIndexes);free(res3.dynamicChangeUpdates);
    
    return finalResult;
}