#include <stdio.h>
#include <stdlib.h>

#include "solver.h"
#include "queue.h"
#include "stack.h"
#include "priorityq.h"
#include "MazeGenerator.h"

// The actual memory allocation for the variables declared in solver.h
Coordinate BFSresult[DEFAULTSIZE];
int BFSresult_size = 0;
Coordinate DFSresult[DEFAULTSIZE];
int DFSresult_size = 0;
Coordinate DJKresult[DEFAULTSIZE];
int DJKresult_size = 0;
Coordinate BFSexplored[DEFAULTSIZE];
int BFSexplored_size = 0;
Coordinate DFSexplored[DEFAULTSIZE];
int DFSexplored_size = 0;
Coordinate DJKexplored[DEFAULTSIZE];
int DJKexplored_size = 0;
Coordinate ASTresult[DEFAULTSIZE];
int ASTresult_size = 0;
Coordinate ASTexplored[DEFAULTSIZE];
int ASTexplored_size = 0;
int dynamicMode = 0;//global variable to check if dynamic mode is enabled or not

int applyTheDynamicChanges(int rows, int cols, int totalCells, int** adjMat, TestResult* res, int* updateIndex, int* changeIndex, int* parent, int current) {
    if (!dynamicMode) return -1;
    if (*updateIndex >= DYNAMICUPDATESIZE - 2) return -1;

    int u = DynamicWallChange(rows, cols, totalCells, adjMat, res->dynamicChangeUpdates, updateIndex, parent, current);
    
    if (u != -1) {
        res->dynamicChangeIndexes[*changeIndex] = res->exploredCount;
        (*changeIndex)++;
    }
    return u;
}

TestResult solve_BFS(int start,int end,int rows,int collumns,int** AdjMatrix){
    TestResult res={"BFS", 0, 0, NULL, 0, NULL, 0, NULL, NULL,0};
    int updateIndex=0,changeIndex=0,frequency=FREQUENCY;
    int u,i;
    int totalCellCount= rows*collumns;
    BFSexplored_size=0;BFSresult_size=0;
    int* visited=(int*)safe_malloc(totalCellCount*sizeof(int));
    for(i=0; i<totalCellCount;i++) visited[i]=0; //hepsi 0
    int* parent = (int*)safe_malloc(totalCellCount * sizeof(int));
    for(i=0;i<totalCellCount;i++) parent[i]= -1; //hepsi -1
    Kuyruk* q = createQueue(totalCellCount);
    visited[start]=1;
    enqueue(q,start);
    int foundFlag = 0;
    res.dynamicChangeUpdates=(MatrixUpdate*)safe_malloc(sizeof(MatrixUpdate)*DYNAMICUPDATESIZE);//en kotu ihtimal 80 değişim iki enum ihtimali var 160 yani ben 200 yaptım garanti için
    res.dynamicChangeIndexes=(int*)safe_malloc(sizeof(int)*DYNAMICINDEXSIZE);//en kotu ihtimal 400/5 değişim olur yani 80 ben 100 koydum garanti olsun diye
    while(!isEmptyQ(q)){
        int current=dequeue(q);
  
        int p=parent[current];
        if (p != -1 && AdjMatrix[p][current] == 0) { // Eğer başlangıç değilse ve geldiği yol artk duvarsa
            visited[current] = 0; // Bu yolu geçersiz say ve hücreyi tekrar aç
            continue;
            }
        BFSexplored[BFSexplored_size].x=current/collumns;
        BFSexplored[BFSexplored_size].y=current%collumns;
        res.exploredCount = ++BFSexplored_size;
        if(dynamicMode&&res.exploredCount%frequency==0){
            u = applyTheDynamicChanges(rows, collumns, totalCellCount, AdjMatrix, &res, &updateIndex, &changeIndex, parent, current);
    }
        if (current==end) {
                foundFlag=1;
                break;
            }
        int neighbor;
        for (neighbor=0; neighbor < totalCellCount; neighbor++) { //tum komsulari dolaşıp daha once ziyaret edilmemis ve degeri 1 olanları quueue'ye al
                if (AdjMatrix[current][neighbor]>0 && !visited[neighbor]) {
                    visited[neighbor] = 1;
                    parent[neighbor]=current;
                    enqueue(q,neighbor);
                }
            }
    }
        res.exploredCount=BFSexplored_size;
        res.explored=(int*)safe_malloc(sizeof(int)*res.exploredCount);
        for(i=0;i<res.exploredCount;i++){
            res.explored[i]=BFSexplored[i].x * collumns + BFSexplored[i].y;
        }
    if(foundFlag){ //bulundugu zaman yapılacak seyler
        int tempPath[COST];
        int temporary=end;
        int totalStepsCount=0;
        int totalWeight=0;
        int p;
        while (temporary!=-1) {
            tempPath[totalStepsCount]=temporary;
            p=parent[temporary];
            
            if (p != -1) {
                totalWeight+=AdjMatrix[p][temporary];
            }
            totalStepsCount++;
            temporary=p;
    }
        res.resultCount = totalStepsCount;
        res.steps=totalStepsCount;
        res.weight=totalWeight;
        BFSresult_size=totalStepsCount;

        for(i=0;i<totalStepsCount;i++){
            int vertexID=tempPath[totalStepsCount-1-i];
            BFSresult[i].x=vertexID/collumns; 
            BFSresult[i].y=vertexID%collumns;
        }
        res.result=(int*)safe_malloc(res.steps*sizeof(int));
        for(i = 0; i < res.steps; i++) {
            res.result[i] = tempPath[res.steps-1-i]; // Düz sıraya çevir
        }
    }
    res.dynamicChangeCount=changeIndex;
    free(visited);free(parent);freeQueue(q);
    return res;

    }

TestResult solve_DFS(int start, int end, int rows, int collumns, int** AdjMatrix){
    TestResult res={"DFS", 0, 0, NULL, 0, NULL, 0, NULL, NULL,0};
    int totalCellCount=rows*collumns;
    int updateIndex = 0, changeIndex = 0, frequency = FREQUENCY;
    DFSexplored_size=0;DFSresult_size=0;
    int* visited = (int*)safe_malloc(totalCellCount*sizeof(int));
    int i,u;
    for(i=0;i<totalCellCount;i++) visited[i]=0; //hepsi 0
    int* parent = (int*)safe_malloc(totalCellCount*sizeof(int));
    for(i=0;i<totalCellCount;i++) parent[i]=-1; //hepsi -1

    res.dynamicChangeUpdates=(MatrixUpdate*)safe_malloc(sizeof(MatrixUpdate)*DYNAMICUPDATESIZE);
    res.dynamicChangeIndexes=(int*)safe_malloc(sizeof(int)*DYNAMICINDEXSIZE);
    Stack* s=createStack(totalCellCount);
    push(s,start);
    visited[start]=1;
    int foundFlag=0;
    while(!isEmptyS(s)){
        int current=pop(s);
        int p=parent[current];
        if (p != -1 && AdjMatrix[p][current] == 0) {
            visited[current] = 0;
            continue;
            }
        DFSexplored[DFSexplored_size].x=current/collumns;
        DFSexplored[DFSexplored_size].y=current%collumns;
        
        res.exploredCount= ++DFSexplored_size;
        if (dynamicMode&&res.exploredCount % frequency == 0) {
            u = applyTheDynamicChanges(rows, collumns, totalCellCount, AdjMatrix, &res, &updateIndex, &changeIndex, parent, current);
        }
        if(current == end){
            foundFlag = 1; // Bu satır olmazsa res.result dolmaz!
            break;
            }

        int neighbour;
        for(neighbour=0;neighbour<totalCellCount;neighbour++){
            if(AdjMatrix[current][neighbour]>0&&!visited[neighbour]){
                visited[neighbour]=1;
                parent[neighbour]=current;
                push(s,neighbour);
            }

        }
    }
    
    res.exploredCount=DFSexplored_size;
    res.explored=(int*)safe_malloc(sizeof(int)*res.exploredCount);
    for(i=0;i<res.exploredCount;i++) {
        res.explored[i] = DFSexplored[i].x * collumns + DFSexplored[i].y;
    }
    if(foundFlag){
        int tempPath[COST];
        int temporary=end;
        int totalStepsCount=0,totalWeight=0,p;
        while (temporary!=-1) { //geriden baslayip en basa geliyorum cunku en bas -1 i gosteriyor
            tempPath[totalStepsCount]=temporary;
            p=parent[temporary];
            if(p!=-1){
                totalWeight+=AdjMatrix[p][temporary];
            }
            totalStepsCount++;
            temporary=p;

        }
        DFSresult_size=totalStepsCount;
        for (i=0;i<totalStepsCount;i++) {
            int vertexID=tempPath[totalStepsCount-1-i];
            DFSresult[i].x=vertexID/collumns; 
            DFSresult[i].y=vertexID%collumns;
        }
        res.resultCount = totalStepsCount;
        res.steps = totalStepsCount;res.weight = totalWeight;res.resultCount = totalStepsCount;  
        res.result=(int*)safe_malloc(sizeof(int)*res.steps);
        for(i = 0; i < res.steps; i++) {
            res.result[i] = tempPath[res.steps - 1 - i];
        }    
    }
    res.dynamicChangeCount=changeIndex;
    free(visited);free(parent);freeStack(s);
    return res;
}
TestResult solve_Dijkstra(int start, int end, int rows, int collumns, int** AdjMatrix) {
    TestResult res ={"Dijkstra", 0, 0, NULL, 0, NULL, 0, NULL, NULL,0};
    int totalCellCount = rows*collumns;
    DJKexplored_size=0;DJKresult_size = 0;
    int updateIndex = 0, changeIndex = 0, frequency = FREQUENCY;
    int i,neighbour;
    int* distance=(int*)safe_malloc(totalCellCount*sizeof(int));
    for(i=0;i<totalCellCount;i++) distance[i]=INF; //hepsi sonsuz

    int* parent=(int*)safe_malloc(totalCellCount * sizeof(int));
    for(i=0; i < totalCellCount; i++) parent[i] =-1; //hepsi -1

    int* visited=(int*)safe_malloc(totalCellCount* sizeof(int));
    for(i=0;i<totalCellCount;i++) visited[i]=0; //hepsi 0

    res.dynamicChangeUpdates = (MatrixUpdate*)safe_malloc(sizeof(MatrixUpdate) * DYNAMICUPDATESIZE);
    res.dynamicChangeIndexes = (int*)safe_malloc(sizeof(int) * DYNAMICINDEXSIZE);

    initHeap();
    distance[start]=0;
    insert(0, start);
    int foundFlag=0;

    while(h.size>0){
        PQNode currentNode=extract();
        int currentVertex=currentNode.vertex;
        if (visited[currentVertex]==1) 
            continue;//zaten ziyaret edilmisse while ı en bastan baslat
        visited[currentVertex]=1;
        DJKexplored[DJKexplored_size].x=currentVertex/collumns;
        DJKexplored[DJKexplored_size].y=currentVertex%collumns;
        DJKexplored_size++;
        res.exploredCount = DJKexplored_size;
        if(dynamicMode&&res.exploredCount>0&&res.exploredCount%frequency==0){
            int dynamicChangedCell=applyTheDynamicChanges(rows, collumns, totalCellCount, AdjMatrix, &res, &updateIndex, &changeIndex, parent, currentVertex);
        }
        if (currentVertex == end) {
            foundFlag=1;
            break;
        }
        int weight;
        for(neighbour=0;neighbour<totalCellCount;neighbour++){
            weight=AdjMatrix[currentVertex][neighbour];
            if(weight>0){
                if(distance[currentVertex]+weight<distance[neighbour]){
                    distance[neighbour]=distance[currentVertex]+weight;
                    parent[neighbour]=currentVertex;
                    insert(distance[neighbour],neighbour);
                }
            }
        }
    }
    res.exploredCount=DJKexplored_size;
    res.explored=(int*)safe_malloc(sizeof(int)*res.exploredCount);
    for(i=0;i<res.exploredCount;i++){
        res.explored[i] = DJKexplored[i].x * collumns + DJKexplored[i].y;
    }
    

    if(foundFlag){
        int tempPath[COST];
        int temporary=end;
        int totalStepsCount=0,totalWeight=0 ;
        int p;
        while (temporary!=-1) {
            tempPath[totalStepsCount]=temporary;
            p=parent[temporary];
            if(p!=-1){
                totalWeight+=AdjMatrix[p][temporary];}
            totalStepsCount++;
            temporary=p;
        }



        DJKresult_size=totalStepsCount;
        for (i=0;i<totalStepsCount;i++) {
            int vertexID = tempPath[totalStepsCount-1-i];
            DJKresult[i].x=vertexID/collumns; 
            DJKresult[i].y=vertexID%collumns;
        }
        res.resultCount = totalStepsCount;
        res.steps=totalStepsCount;
        res.weight=totalWeight;
        res.result = (int*)safe_malloc(sizeof(int) * res.steps);
        for(i = 0; i < res.steps; i++) {
            res.result[i] = tempPath[res.steps - 1 - i];
        }

    }
    res.dynamicChangeCount=changeIndex;
    free(distance);free(parent);free(visited);freeHeap();
    return res;
}

TestResult solve_Astar(int start, int end, int rows, int collumns, int** AdjMatrix){ //g(n):baslangictan su anki hücreye kadar harcanan enereji (ağırlıklar 1 ve 2 arasında değişiyor) h(n)=getPerpendicularDistance(dik uzaklığı alıyor),f(n):h(n)veg(n) toplamıdır
    TestResult res = {"Astar", 0, 0, NULL, 0, NULL, 0, NULL, NULL,0};
    int totalCellCount=rows*collumns;
    int i,j,neighbour;

    ASTexplored_size = 0;ASTresult_size = 0;
    int* realCost=(int*)safe_malloc(totalCellCount*sizeof(int));//realcost= baslangictan su anki hucreye kadar maliyet
    for(i=0;i<totalCellCount;i++)   realCost[i]=INF; //hepsi sonsuz
    int* parent=(int*)safe_malloc(totalCellCount*sizeof(int));
    for(i=0;i<totalCellCount;i++)   parent[i]=-1; //hepsi -1
    int* visited=(int*)safe_malloc(totalCellCount*sizeof(int));
    for(i=0;i<totalCellCount;i++)   visited[i]=0; //hepsi 0

    int updateIndex = 0, changeIndex = 0, frequency = FREQUENCY;
    res.dynamicChangeUpdates = (MatrixUpdate*)safe_malloc(sizeof(MatrixUpdate) * DYNAMICUPDATESIZE);
    res.dynamicChangeIndexes = (int*)safe_malloc(sizeof(int) * DYNAMICINDEXSIZE);

    initHeap();
    realCost[start]=0;
    int perpCost=getPerpendicularDistance(start,end,collumns);
    insert(perpCost,start);
    int foundFlag=0;
    while(h.size>0){
        PQNode currentPQNode=extract();
        int current=currentPQNode.vertex;
        if(visited[current]==1) continue;
        visited[current]=1;
        ASTexplored[ASTexplored_size].x = current / collumns;
        ASTexplored[ASTexplored_size].y = current % collumns;
        ASTexplored_size++;
        res.exploredCount=ASTexplored_size;

        if(dynamicMode&&res.exploredCount>0&&res.exploredCount%frequency==0){
            int dynamicChangedCell=applyTheDynamicChanges(rows, collumns, totalCellCount, AdjMatrix, &res, &updateIndex, &changeIndex, parent, current);
        }

        if(current==end){
            foundFlag=1;
            break;
        }
        for(j=0;j<totalCellCount;j++){
            int weight=AdjMatrix[current][j];
            if(weight>0){
                if(realCost[current]+weight<realCost[j]){
                    realCost[j]=realCost[current]+weight;
                    parent[j]=current;

                    int finalCost=realCost[j]+getPerpendicularDistance(j,end,collumns);
                    insert(finalCost,j);
                }
            }
        }
    }
    res.exploredCount=ASTexplored_size;
    res.explored=(int*)safe_malloc(sizeof(int)*res.exploredCount);
    for (i=0;i<res.exploredCount;i++)
        res.explored[i]=ASTexplored[i].x*collumns+ASTexplored[i].y;

    if(foundFlag){
        int tempPath[COST];
        int temporary=end;
        int totalStepsCount=0,totalWeight=0 ;
        int p;   
        while(temporary!=-1){
            tempPath[totalStepsCount]=temporary;
            p=parent[temporary];
            if(p!=-1)
                totalWeight+=AdjMatrix[p][temporary];
            totalStepsCount++;
            temporary=p;
        }
        res.steps=totalStepsCount;
        res.weight=totalWeight;
        res.resultCount=totalStepsCount;
        res.result=(int*)safe_malloc(sizeof(int)*res.steps);
        for (i = 0; i < res.steps; i++) 
            res.result[i] = tempPath[res.steps-1-i]; 
    }
    res.dynamicChangeCount=changeIndex;
    free(realCost); free(parent); free(visited); freeHeap();
    return res;
}