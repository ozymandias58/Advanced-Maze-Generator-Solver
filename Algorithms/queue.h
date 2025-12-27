#ifndef QUEUE_H
#define QUEUE_H

typedef struct kuyruk {
    int elementCount; 
    int capacity;     
    int front;        
    int rear;         
    int* data;        // vertex
} Kuyruk;

Kuyruk* createQueue(int capacity);    
int isEmptyQ(Kuyruk* k);              
void enqueue(Kuyruk* k, int sayi);  
int dequeue(Kuyruk* k);              
void freeQueue(Kuyruk* k);           

#endif