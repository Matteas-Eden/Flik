#ifndef BFS_H
#define BFS_H

#include <stdlib.h>
#include <string.h>
#include "line_map.h"
#include "map_addons.h"
#include "point.h"
#include "helper.h"

/* Verify a valid path and store it */
void checkPath(point new_paths[MAX_NUM_PATHS][MAX_PATH_LENGTH], int * cnt,
    point * concurrent_path, int length, point p){

    int mapval = getMapValAtPoint(p);
    
    if (mapval == 0){
        markPointAsVisited(p);
        concurrent_path[length] = p;
        memcpy(new_paths[(*cnt)++],concurrent_path,MAX_PATH_LENGTH*sizeof(point));
    }
}

/* Breadth First Search */
void BFS(point start, point finish, point * concurrent_path){

    point paths[MAX_NUM_PATHS][MAX_PATH_LENGTH];
    point new_paths[MAX_NUM_PATHS][MAX_PATH_LENGTH];

    // Initialise all paths as empty
    memset(concurrent_path,EMPTY_VAL,MAX_PATH_LENGTH*sizeof(point));
    int i;
    for (i = 0; i < MAX_NUM_PATHS; i++){
        memset(paths[i],EMPTY_VAL,MAX_PATH_LENGTH*sizeof(point));
        memset(new_paths[i],EMPTY_VAL,MAX_PATH_LENGTH*sizeof(point));
    }

    // Setup initial starting point
    paths[0][0] = start;
    markPointAsVisited(start);
    int length = 1; // relates to concurrent_path
    int cnt = 0;

    while (TRUE){
        cnt = 0;
        int i;
        for (i = 0; i < MAX_NUM_PATHS; i++){

            // Skip empty paths
            if (isEmpty(paths[i],MAX_PATH_LENGTH)) continue;

            // Set the concurrent_path equal to path
            memcpy(concurrent_path,paths[i],MAX_PATH_LENGTH*sizeof(point));

            // Clear path
            //clearArray(paths[i],MAX_PATH_LENGTH);
            memset(paths[i],EMPTY_VAL,MAX_PATH_LENGTH*sizeof(point));

            point p = concurrent_path[length-1];
            
            // Path has been found
            if (p.x == finish.x && p.y == finish.y) {
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

        // Take new paths and put them into paths
        for (i = 0; i < MAX_NUM_PATHS; i++){
            memcpy(paths[i],new_paths[i],MAX_PATH_LENGTH*sizeof(point)); // Copy new_paths to paths
            memset(new_paths[i],EMPTY_VAL,MAX_PATH_LENGTH*sizeof(point)); // Empty new_paths
        }

        length++;

    }

}

#endif