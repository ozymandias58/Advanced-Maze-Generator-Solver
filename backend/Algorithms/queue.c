#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

Kuyruk* createQueue(int capacity){
    Kuyruk* k = (Kuyruk*)malloc(sizeof(Kuyruk));
    k->front=0;k->rear=0;k->elementCount=0;
    k->capacity=capacity;
    k->data = (int*)malloc(sizeof(int) * capacity);
        if (k->data == NULL) {
            printf("Hata queue malloc atamasi gerceklestirilemedi\n");
            exit(1);
        }
    return k;
}
int isEmptyQ(Kuyruk* k) {
    return k->elementCount == 0;
}

void freeQueue(Kuyruk* k) {
    free(k->data);
    free(k);
}

void enqueue(Kuyruk* k,int sayi){
    if(k->elementCount==k->capacity){
        k->capacity*=2;
        k->data = (int*)realloc(k->data, sizeof(int) * k->capacity);        
        if (k->data == NULL) {
            printf("Hata queue realloc atamasi gerceklestirilemedi\n");
            exit(1);
        }
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