#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "cJSON.h"
#include "testResult.h"
void saveJSONtoFile(cJSON *obj);
cJSON *convertTesttoJSON(TestResult test);
void createJSON(int **adjMat, int rows, int cols, TestResult *testArr, int testSize);

#endif