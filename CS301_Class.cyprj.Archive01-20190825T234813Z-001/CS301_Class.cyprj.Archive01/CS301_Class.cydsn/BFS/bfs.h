#ifndef BFS_H
#define BFS_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "line_map.h"
#include "map_addons.h"
#include "point.h"
#include "helper.h"
#include "defines.h"

/* Verify a valid path and store it */
void checkPath(point new_paths[MAX_NUM_PATHS][MAX_PATH_LENGTH], uint8_t * cnt,
    point * concurrent_path, int length, point p){

    uint8_t mapval = getMapValAtPoint(p);
    #ifdef DEBUG
        printf("Map @ (%d,%d) = %d\n", p.x,p.y,mapval); // DEBUG
    #endif

    if (mapval == 0){
        markPointAsVisited(p);
        concurrent_path[length] = p;
        memcpy(new_paths[(*cnt)++],concurrent_path,MAX_PATH_LENGTH*sizeof(point));
    }
}

/* Breadth First Search */
void BFS(point start, point finish, point * concurrent_path){

    point paths[MAX_NUM_PATHS][MAX_PATH_LENGTH] = { 0 };
    point new_paths[MAX_NUM_PATHS][MAX_PATH_LENGTH] = { 0 };
    uint8_t i; // For C90 compatibility

    // Initialise all paths as empty
    memset(concurrent_path,EMPTY_VAL,MAX_PATH_LENGTH*sizeof(point));
    for (i = 0; i < MAX_NUM_PATHS; i++){
        memset(paths[i],EMPTY_VAL,MAX_PATH_LENGTH*sizeof(point));
        memset(new_paths[i],EMPTY_VAL,MAX_PATH_LENGTH*sizeof(point));
    }

    #ifdef DEBUG
        printf("### FINISHED INITIALISATIONS ###\n");
    #endif

    // Setup initial starting point
    paths[0][0] = start;
    markPointAsVisited(start);
    int length = 1; // relates to concurrent_path
    uint8_t cnt = 0;

    #ifdef DEBUG
        printf("### BEGIN MAIN LOOP ###\n"); // DEBUG
        map[start.y][start.x] = 4;
    #endif

    while (TRUE){
        cnt = 0;
        #ifdef DEBUG
            printf("##%d##\n",length);
        #endif
        for (i = 0; i < MAX_NUM_PATHS; i++){

            // Skip empty paths
            if (isEmpty(paths[i],MAX_PATH_LENGTH)) continue;

            #ifdef DEBUG
                printf("[%d]\n",i); // DEBUG
            #endif

            // Set the concurrent_path equal to path
            memcpy(concurrent_path,paths[i],MAX_PATH_LENGTH*sizeof(point));

            #ifdef DEBUG
                printPath(concurrent_path); // DEBUG
            #endif

            // Clear path
            //clearArray(paths[i],MAX_PATH_LENGTH);
            memset(paths[i],EMPTY_VAL,MAX_PATH_LENGTH*sizeof(point));

            point p = concurrent_path[length-1];

            #ifdef DEBUG
                printf("End node: (%d,%d)\n",p.x,p.y); // DEBUG
            #endif

            // Path has been found
            if (p.x == finish.x && p.y == finish.y) {
                #ifdef DEBUG
                    printf("### FOUND PATH ###\n");
                #endif
                return;
            }

            // Check above
            checkPath(new_paths,&cnt,concurrent_path,length,(point){.x=p.x,.y=p.y-1});
            // Check below
            checkPath(new_paths,&cnt,concurrent_path,length,(point){.x=p.x,.y=p.y+1});
            // Check left
            checkPath(new_paths,&cnt,concurrent_path,length,(point){.x=p.x-1,.y=p.y});
            // Check right
            checkPath(new_paths,&cnt,concurrent_path,length,(point){.x=p.x+1,.y=p.y});

        }

        #ifdef DEBUG
            printf("### UPDATING PATHS ###\n"); // DEBUG
        #endif

        // Take new paths and put them into paths
        for (i = 0; i < MAX_NUM_PATHS; i++){
            #ifdef DEBUG
                printPath(new_paths[i]);
            #endif
            memcpy(paths[i],new_paths[i],MAX_PATH_LENGTH*sizeof(point)); // Copy new_paths to paths
            memset(new_paths[i],EMPTY_VAL,MAX_PATH_LENGTH*sizeof(point)); // Empty new_paths
        }

        #ifdef DEBUG
            printf("Number of paths: %d\n",cnt); // DEBUG
            printMap(map);
            blockAndWait(); // DEBUG
        #endif

        length++;

    }

}

///* Wrapper for BFS that gets a route connecting all food pellets in order */
//void getRouteToFood(point start, point path[MAX_PATH_LENGTH]){
//
//    uint8_t i; // C90 is outdated smh
//
//    /* Setup locations to use for BFS */
//    point locations[6] = { 0 };
//    locations[0] = (point){.x = start.x, .y = start.y};
//
//    /* Intialise path */
//    memset(path,EMPTY_VAL,MAX_PATH_LENGTH*sizeof(point));
//
//    /* Setup map for cleaning after BFS is done */
//    uint8_t clean_map[MAP_ROW][MAP_COL] = { 0 };
//
//    for (i = 0; i < MAP_ROW; i++){
//        memcpy(clean_map[i],map[i],sizeof(int)*MAP_COL);
//    }
//
//    uint8_t k; // Wishing it wasn't C90
//
//    for (i = 0; i < 5; i++){
//
//        // Take next location from the food_list
//        locations[i+1] = (point){.x = food_list[i][0], .y = food_list[i][1]};
//
//        // Setup the path to store the BFS result
//        point temp_path[MAX_PATH_LENGTH] = { 0 };
//        memset(temp_path,EMPTY_VAL,MAX_PATH_LENGTH*sizeof(point));
//
//        // Big Fat Search
//        BFS(locations[i],locations[i+1],temp_path);
//
//        /* Clean up map from BFS */
//        for (k = 0; k < MAP_ROW; k++){
//            memcpy(map[k], clean_map[k], sizeof(int)*MAP_COL);
//        }
//
//        appendPath(path,len(path),temp_path);
//    }
//
//}

#endif