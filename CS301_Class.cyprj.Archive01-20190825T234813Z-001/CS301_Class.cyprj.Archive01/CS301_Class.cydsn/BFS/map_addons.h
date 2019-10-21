#ifndef MAP_ADDONS_H
#define MAP_ADDONS_H

// Dimensions of map and paths in map
#define MAP_ROW 15
#define MAP_COL 19
#define MAX_PATH_LENGTH 140
#define MAX_NUM_PATHS 10
#define MAX_NODES 30
#define MAX_STACK_SIZE 20
#define MAX_EDGES 15

// Colours for printing
#define RED "\033[0;31m"
#define BLUE "\033[0;34m"
#define BOLD_GREEN "\033[1;32m"
#define BOLD_YELLOW "\033[1;33m"
#define DEFAULT_COLOUR "\033[0m"

// Labels for vertices on the map
#define UNVISITED 0
#define WALL 1
#define VISITED 2
#define FINAL_PATH 3
#define START_OR_END 4
#define EMPTY_VAL -1

// Command Macros
#define LEFT_TURN -1
#define RIGHT_TURN -2
#define U_TURN -3

// Parameters for creating the command list
#define MAX_COMMAND_LENGTH 50
#define EMPTY_COMMAND 0
#define CMD_NULL -1
#define CMD_RIGHT 0
#define CMD_DOWN 1
#define CMD_LEFT 2
#define CMD_UP 3
#define START_FACING CMD_UP //-------------------Change Orientation here---------------------------------

// Values that decide turning behaviour
#define GO_STRAIGHT 0
#define TURN_LEFT_1 -1
#define TURN_LEFT_2 3
#define TURN_RIGHT_1 1
#define TURN_RIGHT_2 -3

// Includes
#include <stdio.h>
#include "helper.h"
#include "point.h"
#include "line_map.h"
#include "defines.h"

/* Get the length of a path*/
int len(point * path){
    int len = 0;
    int i;
    for (i = 0; i < MAX_PATH_LENGTH; i++){
        if (path[i].x == EMPTY_VAL && path[i].y == EMPTY_VAL) break;
        len++;
    }
    return len;
}

/* Check if a path is empty i.e. full of EMPTY_VAL */
int isEmpty(point * path, int size){
    int i;
    for (i = 0;i<size;i++){
        if (path[i].x != EMPTY_VAL && path[i].y != EMPTY_VAL) return FALSE;
    }
    return TRUE;
}

int isPointOnPath(point p, point * path, int size){
    int i;
    for (i = 0; i < size; i++){
        if (path[i].x == p.x && path[i].y == p.y) return TRUE;   
    }
    return FALSE;
}

/* Print a path */
void printPath(point * path){
    int i;
    for (i = 0; i < MAX_PATH_LENGTH; i++){
        if (path[i].x == EMPTY_VAL && path[i].y == EMPTY_VAL){
            if (i > 0) break;
            return;
        }
        printf("(%d, %d)",path[i].x,path[i].y);
    }
    puts("");
}

/* Prints the commands */
void printCommands(int *commands){
    fputs("Commands: (",stdout);
    int i;
    for (i = 0; i < MAX_COMMAND_LENGTH; i++){
        if (commands[i] == EMPTY_COMMAND){
            if (i > 0) break;
            return;
        }
        switch(commands[i]){
            case LEFT_TURN:
                printf("L");
                break;
            case RIGHT_TURN:
                printf("R");
                break;
            default:
                printf("%d", commands[i]);
                break;
        };
        if (commands[i+1] != EMPTY_COMMAND) printf(",");
    }
    puts(")");
}

/* Now in colour! :O */
void printMap(int map[MAP_ROW][MAP_COL]){

    printf("////////////////BEGIN MAP////////////////\n");

    int m;
    int n;
    for (m = 0; m < MAP_ROW; m++){
        for (n=0; n < MAP_COL; n++){
            switch (map[m][n]){
                case UNVISITED:
                    printf("%d ", map[m][n]);
                    break;
                case WALL:
                    // Prints red
                    printf(RED "%d" DEFAULT_COLOUR " ", map[m][n]);
                    break;
                case VISITED:
                    // Prints blue
                    printf(BLUE "%d" DEFAULT_COLOUR " ", map[m][n]);
                    break;
                case FINAL_PATH:
                    // Prints bold green
                    printf(BOLD_GREEN "%d" DEFAULT_COLOUR " ", map[m][n]);
                    break;
                case START_OR_END:
                    // Prints bold yellow
                    printf(BOLD_YELLOW "%d" DEFAULT_COLOUR " ", map[m][n]);
                    break;
                default:
                    break;
            }
        }
        printf("\n");
    }

    printf("/////////////////END MAP/////////////////\n");

}

/* Modify the map with the final path */
void updateMapWithPath(int map[MAP_ROW][MAP_COL], point * path, int path_length){

    // Set starting position
    map[path[0].y][path[0].x] = 4;

    // Fill up path
    int i;
    for (i = 1; i < path_length-1; i++){
        map[path[i].y][path[i].x] = 3;
    }

    // Set ending position
    map[path[path_length-1].y][path[path_length-1].x] = 4;
}

/* Retrieve the vertex at a specific point on the map */
int getMapValAtPoint(point p){
    if (p.x < 0 || p.x >= MAP_COL) return EMPTY_VAL;
    if (p.y < 0 || p.y >= MAP_ROW) return EMPTY_VAL;
    return map[p.y][p.x];
}

/* Mark a point on the map as visited */
void markPointAsVisited(point p){ map[p.y][p.x] = VISITED; }

/* Interpret coordinates of the final path and convert to commands for robot */
void convertCoordinatesToCommands(point *concurrent_path, int *commands){

    int straight_count = -1;
    point current, next;
    int current_orientation = START_FACING;
    int next_orientation = CMD_NULL;

    int cmd_length = 0;
    int path_length = len(concurrent_path);

    int i;
    for (i = 0; i < path_length - 1; i++){

        current = concurrent_path[i];
        next = concurrent_path[i+1];

        // Since the robot only moves one coord at a time, one of the following branches must evaluate
        if (current.x == next.x){
            next_orientation = (current.y > next.y) ? CMD_UP:CMD_DOWN;
        }
        else if (current.y == next.y){
            next_orientation = (current.x > next.x) ? CMD_LEFT:CMD_RIGHT;
        }

        // Evaluate change (or lack of change) in orientation based on diff
        int diffMod4 = (next_orientation - current_orientation) % 4;

        // Every iteration of the loop, the robot moves one square
        straight_count++;

        // If the robot doesn't need to turn, just skip to the next iteration of the loop (i.e. next square)
        if (diffMod4 == GO_STRAIGHT){
            continue;
        }

        // At this point, the robot must turn, so need to print the count value up until now
        if (straight_count) commands[cmd_length++] = straight_count;

        // A turn has been encountered, add it the commands
        if (diffMod4 == TURN_LEFT_1 || diffMod4 == TURN_LEFT_2) { commands[cmd_length++] = LEFT_TURN; } 
        else if (diffMod4 == TURN_RIGHT_1 || diffMod4 == TURN_RIGHT_2) { commands[cmd_length++] = RIGHT_TURN; }
        else { commands[cmd_length++] = U_TURN; }

        // Reset the count
        straight_count = 0;

        // Update orientation
        current_orientation = next_orientation;

    }

    commands[cmd_length] = straight_count+1;

}

/* Get the number of valid points surrounding a given point on the map */
int getNumOfSurroundingPoints(point p){
    int num, mapval;
    int i;
    int j;
    for (i = p.x-1; i < p.x+2; i++){
        for (j = p.y-1; j < p.y+2; j++){
            if (i == p.x && j == p.y) continue;
            mapval = getMapValAtPoint((point){.x=i,.y=j});
            if (mapval == UNVISITED || mapval == 3 /* different to FINAL_PATH */) 
                num++;
        }
    }

    return num;
}

#endif