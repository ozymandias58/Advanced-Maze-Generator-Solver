#ifndef PRIORITYQ_H
#define PRIORITYQ_H

#define HEAPINITIALCAP 100

typedef struct PQNode{
    int distance;
    int vertex;
}PQNode;

typedef struct MinHeap{
    int capacity;
    int size;
    PQNode* arr;
}MinHeap;
extern MinHeap h;

void initHeap();
void freeHeap();
int getLeftIndex(int index);
int getRightIndex(int index);
int getParentIndex(int index);
int hasLeft(int index);
int hasRight(int index);
int hasParent(int index);
PQNode getLeft(int index);
PQNode getRight(int index);
PQNode getParent(int index);
void swapPQ(PQNode *a, PQNode *b);
void heapifyDown();
void heapifyUp();
PQNode extract();
void insert(int weight, int vertex);
void printHeap();

#endif