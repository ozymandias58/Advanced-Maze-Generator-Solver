#ifndef TEST_RESULT_H
#define TEST_RESULT_H

#include "DynamicWalls.h"

typedef struct TestResult{//Guı a gonderilecek testResult yani res
    //GUI'de gözükecekler
    char* algo; //algoritmanın ismi
    int steps; //adım sayısı
    int weight; //üzerinden geçilen yolların ağırlık toplamı


    //GUI'ye gönderilecekler
    int* explored; //keşfetme sırası (gui için boyama sırası)
    int exploredCount;//kaç hücre keşfedildi
    int* result; //bulunan yolun sırası
    int resultCount;//result yolunda kaç hücre var
    int* dynamicChangeIndexes; //duvarın değiştiği turların sırası (YANİ BOYAMA YAPILIRKEN HANGİ ARALARDA DUVAR DEĞİŞSİN)
    MatrixUpdate* dynamicChangeUpdates; //Duvar değişeceğinde yapılan güncellemeler

}TestResult;

#endif