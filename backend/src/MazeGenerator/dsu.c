#include "dsu.h"
#include "MazeGenerator.h"
#include <stdlib.h>

//initializes dsu with vertice amount
void initDSU(DSU *d, int n){
    d->size = n;
    d->parent = (int*)safe_malloc(sizeof(int)*d->size);
    d->rank = (int*)safe_malloc(sizeof(int)*d->size);
    
    for(int i=0;i<d->size;i++){
        d->parent[i] = i;
        d->rank[i] = 0;
    }
}

//finds the representative of vertice and compresses paths
int find(DSU *d, int i){
    if(d->parent[i] == i)
        return i;
    return d->parent[i] = find(d,d->parent[i]); //path compression
}

//unites two vertices
void unionSet(DSU *d, int u, int v){
    u = find(d,u);
    v = find(d,v);
    if(d->rank[u] < d->rank[v])
        d->parent[u] = v;
    else if(d->rank[u] > d->rank[v])
        d->parent[v] = u;
    else{
        d->parent[v] = u;
        d->rank[u]++;
    }
}