#ifndef SORTING_H
#define SORTING_H

#include "testResult.h"

void qsSwap(TestResult *a, TestResult *b);
int qSortPartition(TestResult *arr, int start, int end);
void quickSortbySteps(TestResult *arr, int start, int end);
void merge(TestResult *arr, int size, TestResult *Larr, int lsize, TestResult *Rarr, int rsize);
void mergeSortbyWeight(TestResult *arr, int size);

#endif