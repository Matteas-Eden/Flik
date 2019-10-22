#ifndef BFS_ALL_H
#define BFS_ALL_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "bfs.h"

// #define DEBUG_ALLPATH
// #define DISPLAY_ALLPATH

int generateTargetPoints(point *targets){

    uint8_t i, j;
    uint16_t size = 0;

    for (i = 0; i < MAP_ROW; i++){
        for (j = 0; j < MAP_COL; j++){
            if (map[i][j] == UNVISITED){
                targets[size] = (point){.x=j,.y=i};
                size++;
            }
        }
    }

    targets[size+1] = (point){.x=EMPTY_VAL,.y=EMPTY_VAL};

    return size;

}

/* Currently deprecated */
// /* Update and remove some targets if they have been traversed */
// int updateTargets(int map[MAP_ROW][MAP_COL], point *targets, int start_index){

//     int i = start_index;

//     while (TRUE){
//         if (targets[i].x == EMPTY_VAL && targets[i].y == EMPTY_VAL) break;

//         if (map[targets[i].y][targets[i].x] == FINAL_PATH
//         || map[targets[i].y][targets[i].x] == START_OR_END){
//             // Remove a target
//             shiftArrayLeft(targets,len(targets),1);
//             continue;
//         }

//         i++;
//     }

//     return i;
// }

/* Get the next point from a pre-generated pool */
point getNextTarget(int map[MAP_ROW][MAP_COL], point * targets){

    uint16_t i = 0;

    while (TRUE){

        if (targets[i].x == EMPTY_VAL && targets[i].y == EMPTY_VAL) break;

        if (map[targets[i].y][targets[i].x] == UNVISITED){
            return targets[i];
        }

        i++;
    }

    return (point){.x = EMPTY_VAL, .y = EMPTY_VAL};

}

/* Just a simple filter to remove garbage values */
void filterPoints(point * points){
    
    uint16_t i = 0;
    
    while (TRUE){

        if (points[i].x >= MAP_COL || points[i].y >= MAP_ROW
        || points[i].x <= 0 || points[i].y <= 0){
            points[i].x = EMPTY_VAL; 
            points[i].y = EMPTY_VAL;
            break;
        }

        if (points[i].x == EMPTY_VAL && points[i].y == EMPTY_VAL) break;

        i++;
    }
}

/* BFS to go everywhere */
/*
* Leverages an alternate version of BFS, which goes until it hits an unvisited point.
* This algorithm then repeats BFS until there are no unvisited points left in the map.
*/
void getAllPath(point start, point allPath[MAX_PATH_LENGTH]){

    int i;

    // blockAndWait();

    #ifdef DEBUG_ALLPATH
        printf("## Plotting paths through map ##\n");
    #endif

    int marked_map[MAP_ROW][MAP_COL];
    int clean_map[MAP_ROW][MAP_COL];

    /* Create copies of map */
    for (i = 0; i < MAP_ROW; i++){
        memcpy(marked_map[i],map[i],sizeof(int)*MAP_COL);
        memcpy(clean_map[i],map[i],sizeof(int)*MAP_COL);
    }

    /* Iterate through all points and update the path */
    int k;
    point curr_point = start;//getNextTarget(marked_map,targets);
    point next_point;
    markPointAsVisited(curr_point);

    while (hasUnvisitedPoints(marked_map)) {

        point path[MAX_PATH_LENGTH] = { 0 };
        next_point = BFS_alternate(curr_point, marked_map, path);

        #ifdef DEBUG_ALLPATH
            printf("### Map w/ (%d,%d)->(%d,%d) ###\n",curr_point.x,curr_point.y,next_point.x,next_point.y);
            updateMapWithPath(map,path,len(path));
            printMap(map);
        #endif

        curr_point = next_point; // update current

        /* Clean up map from BFS */
        for (k = 0; k < MAP_ROW; k++){
            memcpy(map[k], clean_map[k], sizeof(int)*MAP_COL);
        }

        // #ifdef DEBUG_ALLPATH
        //     puts("## Targets ##");
        //     printPath(targets);
        // #endif

        updateMapWithPath(marked_map, path, len(path));
        // num_points = updateTargets(marked_map, targets, i);
        appendPath(allPath, len(allPath), path);

        #ifdef DEBUG_ALLPATH
            printf("Len: %d\n", len(allPath));
            printPath(allPath);
            printMap(marked_map);    
            blockAndWait();
        #endif
        
    }

    //blockAndWait();
    //optimisePath(targets);

    #ifdef DISPLAY_ALLPATH
        printMap(marked_map);
        printf("## Length of All Path: %d ##\n",len(allPath));
        printPath(allPath);
    #endif
}

#endif