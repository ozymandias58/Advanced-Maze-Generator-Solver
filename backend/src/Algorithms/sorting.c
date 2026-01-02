#include "testResult.h"
#include "MazeGenerator.h"

#include <stdlib.h>

void qsSwap(TestResult *a, TestResult *b){
    TestResult t = *a;
    *a = *b;
    *b = t;
}
int qSortPartition(TestResult *arr, int start, int end){
    int pivot = end;
    int i = start-1;
    int j = end;
    for(j=start; j<end; j++){
        if(arr[j].steps<arr[pivot].steps)
            qsSwap(&arr[++i],&arr[j]);
        else continue;
    }
    qsSwap(&arr[++i],&arr[j]);
    return i;
}

//sort by steps
void quickSortbySteps(TestResult* arr, int start, int end){
    if(end<=start) return;
    int pivot = qSortPartition(arr, start, end);

    quickSortbySteps(arr, start, pivot-1);
    quickSortbySteps(arr, pivot+1, end);
}

void merge(TestResult *arr, int size, TestResult *Larr, int lsize, TestResult *Rarr, int rsize){
    int l = 0, r = 0, i = 0;
    while(l < lsize && r<rsize){
        if(Larr[l].weight < Rarr[r].weight)
            arr[i++] = Larr[l++];
        else
            arr[i++] = Rarr[r++];
    }
    while(l<lsize) arr[i++] = Larr[l++];
    while(r<rsize) arr[i++] = Rarr[r++];
}

//sort by weight
void mergeSortbyWeight(TestResult *arr, int size){
    if(size<=1) return;
    int middle = size/2;
    int Lsize = middle;
    int Rsize = size - middle;
    TestResult *leftArr = (TestResult*)safe_malloc(sizeof(TestResult)*Lsize);
    TestResult *rightArr = (TestResult*)safe_malloc(sizeof(TestResult)*Rsize);
    
    int j = 0;
    for(int i = 0; i<size; i++){
        if(i<middle)
            leftArr[i] = arr[i];
        else
            rightArr[j++] = arr[i];
    }
    mergeSortbyWeight(leftArr,Lsize);
    mergeSortbyWeight(rightArr,Rsize);

    merge(arr, size, leftArr, Lsize, rightArr, Rsize);

    free(leftArr);
    free(rightArr);
}