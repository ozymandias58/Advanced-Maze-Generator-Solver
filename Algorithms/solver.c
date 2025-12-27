#include <stdio.h>
#include <stdlib.h>
#include "solver.h"
#include "queue.h"
#include "stack.h"
#include "MazeGenerator.h"
#include "priorityq.h"
#define INF 999999
#define STANDARDWEIGHT 1


void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        printf("Malloc ile yer ayirma hatasi");
        exit(1);
    }
    return ptr;
}

int** convertToAdjMatrix(int rows,int collumns,edge* edgeList, int edgeCount){
    int totalCellCount = rows * collumns;
    int i,j;
    int** matrix = (int**)safe_malloc(totalCellCount * sizeof(int*));
    for (i = 0;i < totalCellCount;i++) {
        // satir icin yer ayir
        matrix[i] = (int*)safe_malloc(totalCellCount * sizeof(int)); 
        // satirin her bir hucresini sıfır yap
        for (j = 0;j < totalCellCount;j++) {
            matrix[i][j] = 0;
        }
    }
    //yollari giriyorum 
    for (i = 0; i < edgeCount; i++) {
        int u = edgeList[i].u;
        int v = edgeList[i].v;
        
        matrix[u][v] = 1; 
        matrix[v][u] = 1;
    }

    return matrix;
}

void solve_BFS(int start,int end,int rows,int collumns,int** AdjMatrix){
    int totalCellCount= rows*collumns;
    BFSexplored_size=0;BFSresult_size=0;
    int* visited=(int*)safe_malloc(totalCellCount*sizeof(int));
    for(int i=0; i<totalCellCount;i++) visited[i]=0; //hepsi 0
    int* parent = (int*)safe_malloc(totalCellCount * sizeof(int));
    for(int i=0;i<totalCellCount;i++) parent[i]= -1; //hepsi -1
    Kuyruk* q = createQueue(totalCellCount);
    visited[start]=1;
    enqueue(q,start);
    int foundFlag = 0;
    while(!isEmptyQ(q)){
    int current=dequeue(q);
    BFSexplored[BFSexplored_size].x=current/collumns;
    BFSexplored[BFSexplored_size].y=current%collumns;
    BFSexplored_size++;
    if (current==end) {
            foundFlag=1;
            break;
        }
    int neighbour;
    for (int neighbor=0; neighbor < totalCellCount; neighbor++) { //tum komsulari dolaşıp daha once ziyaret edilmemis ve degeri 1 olanları quueue'ye al
            if (AdjMatrix[current][neighbor] == 1 && !visited[neighbor]) {
                visited[neighbor] = 1;
                parent[neighbor]=current;
                enqueue(q,neighbor);
            }
        }
    }
    if(foundFlag){ //bulundugu zaman yapılacak seyler
        int tempPath[90000];
        int temporary=end;
        int totalStepsCount=0;
        int i;
        while(temporary!=-1){//geriden baslayip en basa geliyorum cunku en bas -1 i gosteriyor
            tempPath[totalStepsCount++]= temporary;
            temporary =parent[temporary];            
        }
        BFSresult_size=totalStepsCount;

        for(int i=0;i<totalStepsCount;i++){
            int vertexID=tempPath[totalStepsCount-1-i];
            BFSresult[i].x=vertexID/collumns; 
            BFSresult[i].y=vertexID%collumns;
        }
    }
    free(visited);
    free(parent);
    freeQueue(q);
    }

void solve_DFS(int start, int end, int rows, int collumns, int** AdjMatrix){
    int totalCellCount=rows*collumns;
    DFSexplored_size=0;DFSresult_size=0;
    int* visited = (int*)safe_malloc(totalCellCount*sizeof(int));
    int i;
    for(i=0;i<totalCellCount;i++) visited[i]=0; //hepsi 0
    int* parent = (int*)safe_malloc(totalCellCount*sizeof(int));
    for(i=0;i<totalCellCount;i++) parent[i]=-1; //hepsi -1

    Stack* s=createStack(totalCellCount);
    push(s,start);
    visited[start]=1;
    int foundFlag=0;
    while(!isEmptyS(s)){
        int current=pop(s);
        DFSexplored[DFSexplored_size].x=current/collumns;
        DFSexplored[DFSexplored_size].y=current%collumns;
        DFSexplored_size++;
        if(current==end){
            foundFlag=1;
            break;
        }
        int neighbour;
        for(neighbour=0;neighbour<totalCellCount;neighbour++){
            if(AdjMatrix[current][neighbour]==1&&!visited[neighbour]){
                visited[neighbour]=1;
                parent[neighbour]=current;
                push(s,neighbour);
            }

        }
    }
    if(foundFlag){
        int tempPath[90000];
        int temporary=end;
        int totalStepsCount=0;
        while (temporary!=-1) { //geriden baslayip en basa geliyorum cunku en bas -1 i gosteriyor
            tempPath[totalStepsCount++]=temporary;
            temporary=parent[temporary];            
        }
        DFSresult_size=totalStepsCount;
        for (int i=0;i<totalStepsCount;i++) {
            int vertexID=tempPath[totalStepsCount-1-i];
            DFSresult[i].x=vertexID/collumns; 
            DFSresult[i].y=vertexID%collumns;
        }      

    }
    free(visited);
    free(parent);
    freeStack(s);
}
void solve_Dijkstra(int start, int end, int rows, int collumns, int** AdjMatrix) {
    int totalCellCount = rows*collumns;
    DJKexplored_size=0;DJKresult_size = 0;
    int i;
    int* distance=(int*)safe_malloc(totalCellCount*sizeof(int));
    for(i=0;i<totalCellCount;i++) distance[i]=INF; //hepsi sonsuz

    int* parent=(int*)safe_malloc(totalCellCount * sizeof(int));
    for(i=0; i < totalCellCount; i++) parent[i] =-1; //hepsi -1

    int* visited=(int*)safe_malloc(totalCellCount* sizeof(int));
    for(i=0;i<totalCellCount;i++) visited[i]=0; //hepsi 0

    initHeap();
    distance[start]=0;
    insert(0, start);
    int foundFlag=0;

    while(h.size>0){
        PQNode currentNode=extract();
        int currentVertex=currentNode.vertex;
        if (visited[currentVertex]==1) continue;//zaten ziyaret edilmisse while ı en bastan baslat
        visited[currentVertex]=1;
        DJKexplored[DJKexplored_size].x=currentVertex/collumns;
        DJKexplored[DJKexplored_size].y=currentVertex%collumns;
        DJKexplored_size++;
        if (currentVertex == end) {
            foundFlag=1;
            break;
        }
        int neighbour;
        for(neighbour=0;neighbour<totalCellCount;neighbour++){
            if(AdjMatrix[currentVertex][neighbour]==1){
            if (distance[currentVertex]+STANDARDWEIGHT<distance[neighbour]) {
                    distance[neighbour] = distance[currentVertex] +STANDARDWEIGHT;
                    parent[neighbour] = currentVertex;
                    insert(distance[neighbour], neighbour); //yeni mesafeyi kuyruga ekle
                }
            }
        }
    }

    if(foundFlag){
        int tempPath[90000];
        int temporary=end;
        int totalStepsCount=0;
        while (temporary!=-1) {
            tempPath[totalStepsCount++]=temporary;
            temporary=parent[temporary];            
        }
        DJKresult_size=totalStepsCount;
        for (int i=0;i<totalStepsCount;i++) {
            int vertexID = tempPath[totalStepsCount-1-i];
            DJKresult[i].x=vertexID/collumns; 
            DJKresult[i].y=vertexID%collumns;
        }
    }
    free(distance);
    free(parent);
    free(visited);
    freeHeap();
}



