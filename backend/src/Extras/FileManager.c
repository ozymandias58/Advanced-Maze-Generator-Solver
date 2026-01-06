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

cJSON* get_json_from_file(char *filename) {
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