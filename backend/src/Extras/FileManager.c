#include "FileManager.h"
#include "testResult.h"
#include "cJSON.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void saveJSONtoFile(cJSON* obj){
    char *renderedJSON = cJSON_PrintUnformatted(obj); //convert json to string

    //unique name based on time
    time_t now = time(NULL);
    char fullPath[256];
    sprintf(fullPath, "docs/SavedTests/test_%ld.json", (long)now);
    
    FILE *filePtr = fopen(fullPath, "w");
    if (filePtr != NULL) {
        fprintf(filePtr, "%s", renderedJSON); //print renderedJSON to file
        fclose(filePtr);
}

cJSON_Delete(obj); 
free(renderedJSON);
}

cJSON* convertTesttoJSON(TestResult test){
    cJSON *testObj = cJSON_CreateObject();
    if (testObj == NULL) return NULL;
    cJSON_AddStringToObject(testObj,"name",test.algo ? test.algo : "unknown");
    cJSON_AddNumberToObject(testObj,"steps",test.steps);
    cJSON_AddNumberToObject(testObj,"weight",test.weight);
    cJSON* explored = cJSON_CreateIntArray(test.explored,test.exploredCount);
    cJSON_AddItemToObject(testObj,"explored",explored);
    cJSON_AddNumberToObject(testObj,"exploredSize",test.exploredCount);
    cJSON* result = cJSON_CreateIntArray(test.result,test.resultCount);
    cJSON_AddItemToObject(testObj,"path",result);
    cJSON_AddNumberToObject(testObj,"pathSize",test.resultCount);

    return testObj;
}

void createJSON(int **adjMat,int rows, int cols, TestResult* testArr, int testSize,int option_weight, int option_dynamic, int option_multi){
    int totalCells = rows*cols;
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root,"file_type","MAZE_SOLVER_DATA");

    cJSON *options = cJSON_CreateObject();
    cJSON_AddNumberToObject(options,"weight",option_weight);
    cJSON_AddNumberToObject(options,"dynamic",option_dynamic);
    cJSON_AddNumberToObject(options,"multi",option_multi);

    cJSON_AddItemToObject(root,"options",options);

    cJSON *jsonMatrix = cJSON_CreateArray(); //create the matrix
    for(int i=0; i<totalCells; i++){
        cJSON *jsonRowArr = cJSON_CreateIntArray(adjMat[i],totalCells);
        cJSON_AddItemToArray(jsonMatrix,jsonRowArr); //add the rows to the matrix
    }
    cJSON_AddItemToObject(root,"graph", jsonMatrix);

    cJSON *jsonTestArr = cJSON_CreateArray();
    for(int i=0; i<testSize;i++){
        cJSON* test = convertTesttoJSON(testArr[i]);
        cJSON_AddItemToArray(jsonTestArr,test);
    }
    cJSON_AddItemToObject(root,"tests",jsonTestArr);
    saveJSONtoFile(root);
}

cJSON* getJSONFromFile(char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *data = (char*)malloc(length + 1);
    if (data == NULL) {
        fclose(file);
        return NULL;
    }
    fread(data, 1, length, file);
    data[length] = '\0';
    fclose(file);

    cJSON *root = cJSON_Parse(data);
    
    free(data);

    if (root == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        return NULL;
    }

    cJSON *type = cJSON_GetObjectItem(root, "file_type");
    if (!cJSON_IsString(type) || strcmp(type->valuestring, "MAZE_SOLVER_DATA") != 0) {
        printf("Validation Error: This file is not a valid maze solver JSON!\n");
        cJSON_Delete(root);
        return NULL;
    }

    return root;
}

void readFromJSON(cJSON *root, int **adjmat, int rows, int cols, TestResult** testArr, int* testArrSize, int* option_weight, int* option_dynamic, int* option_multi){
    cJSON *options = cJSON_GetObjectItem(root, "options");
    if(cJSON_IsObject(options)){
        *option_weight = cJSON_GetObjectItem(options, "weight")->valueint;
        *option_dynamic = cJSON_GetObjectItem(options, "dynamic")->valueint;
        *option_multi = cJSON_GetObjectItem(options, "multi")->valueint;
    }

    int totalCells = rows * cols;

    cJSON *graph = cJSON_GetObjectItem(root, "graph");
    if(cJSON_IsArray(graph)){
        for(int i = 0; i < totalCells; i++){
            cJSON *row = cJSON_GetArrayItem(graph, i);
            for(int j = 0; j < totalCells; j++){
                adjmat[i][j] = cJSON_GetArrayItem(row, j)->valueint;
            }
        }
    }

    cJSON *tests = cJSON_GetObjectItem(root, "tests");
    if(cJSON_IsArray(tests)){
        *testArrSize = cJSON_GetArraySize(tests);
        
        *testArr = (TestResult*)malloc(sizeof(TestResult) * (*testArrSize));

        for(int i = 0; i < *testArrSize; i++){
            cJSON *item = cJSON_GetArrayItem(tests, i);
            TestResult *t = &((*testArr)[i]);

            t->algo = strdup(cJSON_GetObjectItem(item, "name")->valuestring);
            t->steps = cJSON_GetObjectItem(item, "steps")->valueint;
            t->weight = cJSON_GetObjectItem(item, "weight")->valueint;

            t->exploredCount = cJSON_GetObjectItem(item, "exploredSize")->valueint;
            t->explored = (int*)malloc(sizeof(int) * t->exploredCount);
            cJSON *explJson = cJSON_GetObjectItem(item, "explored");
            for(int j = 0; j < t->exploredCount; j++){
                t->explored[j] = cJSON_GetArrayItem(explJson, j)->valueint;
            }

            t->resultCount = cJSON_GetObjectItem(item, "pathSize")->valueint;
            t->result = (int*)malloc(sizeof(int) * t->resultCount);
            cJSON *pathJson = cJSON_GetObjectItem(item, "path");
            for(int j = 0; j < t->resultCount; j++){
                t->result[j] = cJSON_GetArrayItem(pathJson, j)->valueint;
            }
        }
    }
}

void freeTestResults(TestResult* testArr, int testSize) {
    if (testArr == NULL) return;

    for (int i = 0; i < testSize; i++) {
        if (testArr[i].algo) free(testArr[i].algo);

        if (testArr[i].explored) free(testArr[i].explored);
        if (testArr[i].result) free(testArr[i].result);
        
    }

    free(testArr);
}