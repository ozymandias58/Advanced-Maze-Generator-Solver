#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#define MULTIPLICATIONNUMBER 2

Stack* createStack(int capacity){
    Stack* s = (Stack*)malloc(sizeof(Stack));
    s->capacity=capacity;
    s->top=-1;
    s->data = (int*)malloc(sizeof(int) * capacity);
    if (s->data == NULL) {
        printf("Hata Stack malloc ataması basarisiz\n");
        exit(1);
    }
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
        s->data = (int*)realloc(s->data, sizeof(int) * s->capacity);
        if (s->data == NULL) {
            printf("Hata Stack realloc atamasi basarısiz\n");
            exit(1);
        }
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