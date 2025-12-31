#ifndef TEST_RESULT_H
#define TEST_RESULT_H

typedef struct TestResult{
    char* algo;
    int steps;
    int weight;
    int* eplored;
    int* result;
    int* dynamicChange;
}TestResult;

#endif