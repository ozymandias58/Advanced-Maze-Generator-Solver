#include "priorityq.h"
#include "MazeGenerator.h"
#include <stdio.h>
#include <stdlib.h>

//initHeap() first
//insert(weight,vertex) and extract() as much as you want
//freeHeap() after you are done

MinHeap h;

void initHeap(){
    h.capacity = HEAPINITIALCAP;
    h.size = 0;
    h.arr = (PQNode*)safe_malloc(sizeof(PQNode)*h.capacity);
}

void freeHeap(){
    free(h.arr);
}

int getLeftIndex(int index){
    return (index*2)+1;}
int getRightIndex(int index){
    return (index*2)+2;}
int getParentIndex(int index){
    return (index-1)/2;}
int hasLeft(int index){
    return getLeftIndex(index) < h.size;}
int hasRight(int index){
    return getRightIndex(index) < h.size;}
int hasParent(int index){
    return getParentIndex(index) < h.size;}
PQNode getLeft(int index){
    return h.arr[getLeftIndex(index)];}
PQNode getRight(int index){
    return h.arr[getRightIndex(index)];}
PQNode getParent(int index){
    return h.arr[getParentIndex(index)];}

void swapPQ(PQNode *a, PQNode *b){
    PQNode t = *a;
    *a = *b;
    *b = t;
}

void heapifyDown(){
    int index = 0;
    while(hasLeft(index)){
        int smallerChildIndex = getLeftIndex(index);
        if(hasRight(index) && getRight(index).distance < h.arr[smallerChildIndex].distance)
            smallerChildIndex = getRightIndex(index);
        if(h.arr[smallerChildIndex].distance > h.arr[index].distance)
            break;
        swapPQ(&(h.arr[index]),&(h.arr[smallerChildIndex]));
        index = smallerChildIndex;
    }
}

void heapifyUp(){
    int index = h.size-1;
    while(hasParent(index) && getParent(index).distance > h.arr[index].distance){
        swapPQ(&(h.arr[index]),&(h.arr[getParentIndex(index)]));
        index = getParentIndex(index);
    }
}

PQNode extract(){
    if(h.size == 0){
        printf("\n priority queue is empty");
        PQNode emptyNode = {-1,-1};
        return emptyNode ;}
    PQNode item = h.arr[0];
    h.arr[0] = h.arr[h.size-1];
    h.size--;
    heapifyDown();
    return item;
}

void insert(int weight, int vertex){
    if(h.size == h.capacity){
        h.capacity *= 2;
        h.arr = safe_realloc(h.arr,sizeof(PQNode)*h.capacity);
    }
    PQNode node = {weight,vertex};
    h.arr[h.size++] = node;
    heapifyUp();
}

void printHeap(){
    if(h.size == 0)
        return;
        printf("\n Priority Queue: ");
    for(int i=0; i<h.size;i++){
        printf("%d, ", h.arr[i].distance);
    }
    printf("\n");
}