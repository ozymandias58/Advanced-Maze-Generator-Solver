#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "cJSON.h"
#include "testResult.h"

void saveJSONtoFile(cJSON *obj);
cJSON *convertTesttoJSON(TestResult test);
void createJSON(int **adjMat, int rows, int cols, TestResult *testArr, int testSize, int option_weight, int option_dynamic, int option_multi);
cJSON *getJSONFromFile(char *filename);
void readFromJSON(cJSON *root, int **adjmat, int rows, int cols, TestResult **testArr, int *testArrSize, int *option_weight, int *option_dynamic, int *option_multi);
void freeTestResults(TestResult* testArr, int testSize);
#endif