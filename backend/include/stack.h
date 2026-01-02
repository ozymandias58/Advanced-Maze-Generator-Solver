#ifndef STACK_H
#define STACK_H

typedef struct stack{// stack struct
    int top;
    int* data;
    int capacity;
}Stack;

Stack* createStack(int capacity);//stack olustur
int isEmptyS(Stack* s) ;//stack bos mu?
void freeStack(Stack* s) ;//stacki free yap
void push(Stack* s,int sayi);//torbaya pushla
int pop(Stack* s);//torbadan cikar

#endif
