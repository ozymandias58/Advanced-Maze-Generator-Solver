#ifndef DSU_H
#define DSU_H

typedef struct DSU{
    int* parent;
    int* rank;
    int size;
}DSU;

void initDSU(DSU *d, int n);

int find(DSU *d, int i);

void unionSet(DSU *d, int u, int v);
#endif