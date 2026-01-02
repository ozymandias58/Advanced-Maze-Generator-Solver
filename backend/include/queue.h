#ifndef QUEUE_H
#define QUEUE_H

typedef struct kuyruk {
    int elementCount; 
    int capacity;     
    int front;        
    int rear;         
    int* data;        // vertex
} Kuyruk;

Kuyruk* createQueue(int capacity);//kuyruk olustur
int isEmptyQ(Kuyruk* k);//kuyruk bos mu         
void enqueue(Kuyruk* k, int sayi); //kuyruga al 
int dequeue(Kuyruk* k);      //kuyruktan cikar        
void freeQueue(Kuyruk* k); //#freekuyruk          

#endif