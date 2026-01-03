#include "FileManager.h"
#include "testResult.h"
#include "cJSON.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void saveJSONtoFile(cJSON* obj){
    char *renderedJSON = cJSON_Print(obj); //convert json to string

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

void createJSON(int **adjMat,int rows, int cols, TestResult* testArr, int testSize){
    int totalCells = rows*cols;
    cJSON *root = cJSON_CreateObject();

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