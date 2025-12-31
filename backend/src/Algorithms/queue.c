#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "MazeGenerator.h"

Kuyruk* createQueue(int capacity){
    Kuyruk* k =(Kuyruk*)safe_malloc(sizeof(Kuyruk));
    k->front=0;k->rear=0;k->elementCount=0;
    k->capacity=capacity;
    k->data=(int*)safe_malloc(sizeof(int) * capacity);
    return k;
}
int isEmptyQ(Kuyruk* k) {
    return k->elementCount==0;
}

void freeQueue(Kuyruk* k) {
    free(k->data);
    free(k);
}

void enqueue(Kuyruk* k,int sayi){
    if(k->elementCount==k->capacity){
        k->capacity*=2;
        k->data=(int*)safe_realloc(k->data,sizeof(int)*k->capacity);        
    }
    k->data[k->rear] = sayi;
    k->rear++;
    k->elementCount++;
}
int dequeue(Kuyruk* k) {
    if (k->elementCount == 0){
        printf("Hata kuyruk bos dequeue basarisiz");
        return -1;
    } 
    int sayi = k->data[k->front];
    k->front++;
    k->elementCount--;
    return sayi;
}