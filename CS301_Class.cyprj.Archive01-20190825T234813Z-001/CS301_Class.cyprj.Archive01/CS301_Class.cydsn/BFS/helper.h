#ifndef HELPER_H
#define HELPER_H
#include <stdlib.h>
#include <stdio.h>
#include "point.h"
#include "helper.h"

void blockAndWait(){
    char c = getchar();
}

int max(int * arr, int size){
    int max = 0;
    int i;
    for (i = 0; i < size; i++){
        if (arr[i] > max) max = arr[i];
    }
    return max;
}

void reverseRoute(point * arr, int size){
    int i;
    for (i = 0; i < size/2; i++){
        arr[i] = arr[size-i];
    }
}

int isRouteEqual(point * arr1, point * arr2, int size){
    int i;
    for (i = 0; i < size; i++){
        if (arr1[i].x != arr2[i].x || arr1[i].y != arr2[i].y) return FALSE;
    }
    return TRUE;
}

void appendArray(void *source, void *addition){
    // TODO: Implement
}

#endif