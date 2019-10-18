#ifndef HELPER_H
#define HELPER_H
#include <stdlib.h>
#include <stdio.h>
#include "point.h"

// Booleans
#define FALSE 0
#define TRUE !FALSE

void blockAndWait(){
    char c = getchar();
}

/* Standard implementation of itoa */
char* itoa(int val, int base){
	
	static char buf[32] = {0};
	
	int i = 30;
	
	for(; val && i ; --i, val /= base)
	
		buf[i] = "0123456789abcdef"[val % base];
	
	return &buf[i+1];
	
}

int max(int * arr, int size){
    int max = 0;
    for (int i = 0; i < size; i++){
        if (arr[i] > max) max = arr[i];
    }
    return max;
}

void reverseRoute(point * arr, int size){
    for (int i = 0; i < size/2; i++){
        arr[i] = arr[size-i];
    }
}

int isRouteEqual(point * arr1, point * arr2, int size){
    for (int i = 0; i < size; i++){
        if (arr1[i].x != arr2[i].x || arr1[i].y != arr2[i].y) return FALSE;
    }
    return TRUE;
}

void appendArray(void *source, void *addition){
    // TODO: Implement
}

#endif