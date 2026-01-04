#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "MazeGenerator.h"

#define MULTIPLICATIONNUMBER 2

Stack* createStack(int capacity){
    Stack* s = (Stack*)safe_malloc(sizeof(Stack));
    s->capacity=capacity;
    s->top=-1;
    s->data = (int*)safe_malloc(sizeof(int) * capacity);
    return s;
}

int isEmptyS(Stack* s) {
    return s->top == -1;
}

void freeStack(Stack* s) {
    free(s->data);
    free(s);
}

void push(Stack* s,int sayi){
    if (s->top == s->capacity-1){
        s->capacity *= MULTIPLICATIONNUMBER; 
        s->data = (int*)safe_realloc(s->data, sizeof(int) * s->capacity);
    }
    s->top++;
    s->data[(s->top)] = sayi;
}

int pop(Stack* s){
    if (s->top == -1) {
        printf("Hata pop yapılamadi Stack bos\n");
        return -1;
    }
    int temp=s->top;
    s->top--;
    return s->data[temp];
}