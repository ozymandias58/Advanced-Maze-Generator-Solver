#ifndef STACK_H
#define STACK_H

typedef struct stack{
    int top;
    int* data;
    int capacity;
}Stack;

Stack* createStack(int capacity);
int isEmptyS(Stack* s) ;
void freeStack(Stack* s) ;
void push(Stack* s,int sayi);
int pop(Stack* s);

#endif
