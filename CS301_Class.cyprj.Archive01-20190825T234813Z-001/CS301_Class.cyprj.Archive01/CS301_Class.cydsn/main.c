/* ========================================
 * 2019 COMPSYS 
 * 302 GROUP 2
 * FLIK
*/
// ================== INCLUDE ======================
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <project.h>

#include "defines.h"
#include "vars.h"
#include "BFS/bfs.h"
#include "BFS/bfs_allpaths.h"
// =================== MACROS =====================
// SENSOR MAPPINGS
#define TOP_LEFT_SENSOR 0
#define TOP_MID_SENSOR 1
#define TOP_RIGHT_SENSOR 2
#define BOTTOM_LEFT_SENSOR 3
#define BOTTOM_MID_SENSOR 4
#define BOTTOM_RIGHT_SENSOR 5

// LIGHT SENSORS
#define NUM_SENSORS 6
#define LIGHT_LEVEL 300

// ADC
#define ADC_READINGS_SIZE 200

// Speed calculations
#define LINEAR_DISTANCE_PER_REV 20.42035225
#define TICKS_PER_REV 228
#define TIMER_PERIOD 0.03

// Speed control
#define DESIRED_COUNT 8
#define TURN_SPEED 450

// Distances
#define GRID_SIZE 12
#define HALF_ROBOT_LENGTH 8
#define SMALL_FORWARD_DISTANCE 8

// Line following macros - require calibration
#define ADJUST_SPEED_LARGE 3
#define ADJUST_SPEED_SMALL 1
#define MAX_SPEED 9
#define MIN_SPEED 4
#define SLOW_DOWN_SPEED 1

// algorithms
#define DISTANCE_THRESHOLD 20
#define BFS_TEST 1
#define FOOD_ROUTE 2
#define TRAVERSE_ALL 3
#define DEBUG_MODE 4
#define CURRENT_MODE TRAVERSE_ALL

#define START_X 1
#define START_Y 1
#define FINISH_X 17
#define FINISH_Y 13

// Debug flag - uncomment when debugging
#define PUTTY
//#define GO_SLOW
//* ================= FUNCTIONS =======================
void usbPutString(char *s);
void getRouteToFood(point start, point path[MAX_PATH_LENGTH]);

// sensors
int32 getValForChannel(int16 n);
void updateSensorValues();

// general
void changeRightWheelSpeed(int amount);
void changeLeftWheelSpeed(int amount);
void invertWheels();
void startWheels();
void stopWheels();
void setWheelDirection(int isLeftForward, int isRightForward);
void correctSpeed(int prevCount, int count, int desired_count, int isLeftWheel);
float getDistance(int prevCountM1, int prevCountM2);

// straight
void turnRight();
void turnLeft();
void goStraightForBlock(int desired_blocks, int *right_speed_count, int *left_wheel_count);
void goForwardWithoutTracking(int *left_wheel_count, int *right_wheel_count, int distance_to_travel);

// turning
void turnForDegrees(int degrees);
void sharpTurnRight();
void sharpTurnLeft();
void uTurn(int *left_wheel_count, int *right_wheel_count);

// debugging
void printCommandsUSB(int8_t *commands);
void printSingleCommandUSB(int8_t command);
//* ================= GLOBAL VARIABLES =======================
// ADC
volatile int adc_flag = FALSE;
int8 sensor_readings[NUM_SENSORS] = { 0 };
int adc_readings[NUM_SENSORS] = { 0 };
int sample_count = 0;
// Encoder
volatile int timer_flag = FALSE;
volatile int countM1 = 0;
volatile int countM2 = 0;
int prevCountM1 = 0;
int prevCountM2 = 0;
//* ================== ISR ======================
CY_ISR(ENCODER_ISR) {
    countM1 = QuadDec_M1_GetCounter();
    countM2 = QuadDec_M2_GetCounter();
    timer_flag = TRUE;
}

/* ### Retrieve and Store Measurements ### */
CY_ISR(ADC_ISR) {
    adc_flag = TRUE;
}

/** ## Notes
    
    Ratio of linear distance to quad decoder ticks
    One revolution gives a linear distance (in cm)
    Each revolution is 57 ticks (228 for 4x mode)
    Centimetres per tick = linear distance / countM(1|2)
    
    Decoder use a 4x count, so each revolution gives 4 counts
    Sampling rate is determined by our 100ms timer (might be a little slow)
    
    ## Algorithm Control Flow
    - Get ADC value for each channel
    - Get decoder val for M1 & M2
    - Use these values to drive the movement of the robot
    
*/

int main()
{

    // delay
    CyDelay(2000);
    
    // --------------------------------
    // ----- INITIALIZATIONS ----------
    // ================================
    CYGlobalIntEnable;
    
    // ------------ USB SETUP ------------    
    #ifdef USE_USB    
        USBUART_Start(0, USBUART_5V_OPERATION);
    #endif
    
    // PWMs
    PWM_1_Start();
    PWM_2_Start();
    
    stopWheels();
    
    // Timer for encoder
    Timer_TS_Start();
    isr_TS_Start(); // Timer interrupts need to be started independently
    isr_TS_StartEx(ENCODER_ISR);
    
    // Quad Decoders
    QuadDec_M1_Start();
    QuadDec_M2_Start();
    
    // ADC
    ADC_Start(); // Start ADC
    ADC_IRQ_Enable(); // Enable ADC interrupts
    ADC_StartConvert(); // Start conversions
    isr_eoc_StartEx(ADC_ISR); // link interrupt for ADC
    
    // To make sure wheels rotate in the same direction i.e. forward
    INV2_Write(1);
    
    int right_wheel_count = DESIRED_COUNT;
    int left_wheel_count = DESIRED_COUNT;
    
    //int directions[30] = {2, RIGHT_TURN, 2, RIGHT_TURN, 4, RIGHT_TURN,2, LEFT_TURN, 4, LEFT_TURN, 2, LEFT_TURN, 2 , RIGHT_TURN, 4, U_TURN, 2, LEFT_TURN, 4, RIGHT_TURN, 2, LEFT_TURN, 2 , RIGHT_TURN, 2 , LEFT_TURN, 4 };
    //int8_t directions[2] = {RIGHT_TURN, EMPTY_COMMAND};
    
    #ifdef PUTTY
        usbPutString("## Testing Algorithm ##\r\n");
    #endif
    
    // Change these for pathing
    point start = {.x = START_X, .y = START_Y};         //17,5; 1,1; 3,13
    point destination = {.x = FINISH_X, .y = FINISH_Y}; //3,13; 17,13;11,13
    point route[MAX_PATH_LENGTH] = { 0 };               //facing up; right; left
    route[0] = (point){.x = EMPTY_VAL, .y = EMPTY_VAL};
    int8_t directions[MAX_COMMAND_LENGTH] = { 0 };
    
    uint8_t mode = CURRENT_MODE;
    
    LED_ON; // Indicates algorithm starting
    CyDelay(500);
    
    switch (mode){
        case BFS_TEST:
            usbPutString("## Selected BFS_TEST\r\n");
            BFS(start, destination, route);
            break;
        case FOOD_ROUTE:
            usbPutString("## Selected FOOD_ROUTE\r\n");
            getRouteToFood(start,route);
            break;
        case TRAVERSE_ALL:
            usbPutString("## Selected TRAVERSE_ALL\r\n");
            getAllPath(start,route);
            break;
        case DEBUG_MODE:
            startWheels();
            sharpTurnRight();
            PWM_1_WriteCompare(0);
            PWM_2_WriteCompare(0);
            return 0;
            break;
    }
    
    convertCoordinatesToCommands(route,directions);
    printCommandsUSB(directions);
    
    LED_OFF; // Indicates algorithm finishing
    CyDelay(500);
    
    startWheels();
        
    int direction_index = 0;
    while (directions[direction_index] != 0) {
        
        if (directions[direction_index] > DISTANCE_THRESHOLD) break;
        
        if (directions[direction_index] == RIGHT_TURN) {
            sharpTurnRight();
        } else if (directions[direction_index] == LEFT_TURN) {
            sharpTurnLeft();
        } else if (directions[direction_index] == U_TURN) {
            uTurn(&right_wheel_count, &left_wheel_count);
        } else if (directions[direction_index] <= DISTANCE_THRESHOLD){
            int num_coords = directions[direction_index];
            goStraightForBlock(num_coords, &right_wheel_count, &left_wheel_count);
        }
        
        #ifdef GO_SLOW
            stopWheels();
            printSingleCommandUSB(directions[direction_index]);
            CyDelay(1000);
            startWheels();
        #endif
        
        direction_index++;
    }
    
    goForwardWithoutTracking(&left_wheel_count, &right_wheel_count, SMALL_FORWARD_DISTANCE);
    
    LED_Write(1);
    
    PWM_1_WriteCompare(0);
    PWM_2_WriteCompare(0);
    
    // delay
    CyDelay(20000);
    
    return 0;
}

// --------------------------------------------- ADC ------------------------------------------
// Simple ADC conversion
int32 getValForChannel(int16 n){
    return ADC_CountsTo_mVolts(ADC_GetResult16(n));
}

void updateSensorValues(){
    // update each sensor with the maximum reading so far
    int i;
    for (i = 0; i < NUM_SENSORS; i++){
        int reading = getValForChannel(i);
        if (reading > adc_readings[i]) {
            adc_readings[i] = reading;
        }
    }

    char buff[32];
    
    sample_count++;
    if (sample_count == ADC_READINGS_SIZE){
        // Black = 1 (high voltage level)
        for (i = 0; i < NUM_SENSORS; i++){
            sensor_readings[i] = !!(adc_readings[i] < LIGHT_LEVEL);
            adc_readings[i] = 0; // Reset max value from samples
        }
        
        ADC_DEBUG_Write(1);
        sample_count = 0;
    }
    else ADC_DEBUG_Write(0);
    
}

// --------------------------------------------- STRAIGHT ------------------------------------------
void goStraightForBlock(int desired_blocks, int *right_wheel_count, int *left_wheel_count) {
    
    *right_wheel_count = DESIRED_COUNT;//TODO: check if needed
    *left_wheel_count = DESIRED_COUNT;
    
    float distance = 0;
    int desired_distance = GRID_SIZE * desired_blocks - HALF_ROBOT_LENGTH;
    
    while (distance < desired_distance) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
        
         // New count values from encoder are ready
        if (timer_flag){
            isr_TS_Disable();
            
            // update distance
            distance += getDistance(prevCountM1, prevCountM2);
            
            // Correct speed
            correctSpeed(prevCountM1,countM1,*left_wheel_count,TRUE);
            correctSpeed(prevCountM2,countM2,*right_wheel_count,FALSE);
            
            // Update previous values
            prevCountM1 = countM1;
            prevCountM2 = countM2;
            
            // Reset flag
            timer_flag = FALSE;
            isr_TS_Enable();
        }
        
        // follow a line algorithm
        if (sensor_readings[TOP_MID_SENSOR]) { // on line
            if (!sensor_readings[TOP_LEFT_SENSOR] && sensor_readings[TOP_RIGHT_SENSOR]) {// Deviated left; want to go right
                *right_wheel_count -= ADJUST_SPEED_SMALL;
                *left_wheel_count += ADJUST_SPEED_SMALL;
            } else if (sensor_readings[TOP_LEFT_SENSOR] && !sensor_readings[TOP_RIGHT_SENSOR]) { // Deviated right; want to go left
                *right_wheel_count += ADJUST_SPEED_SMALL;
                *left_wheel_count -= ADJUST_SPEED_SMALL;
            }
        } else { // moved off
            if (!sensor_readings[TOP_LEFT_SENSOR] && sensor_readings[TOP_RIGHT_SENSOR]) {// Deviated left, but worse; want to go right
                turnRight();
            } else if (sensor_readings[TOP_LEFT_SENSOR] && !sensor_readings[TOP_RIGHT_SENSOR]) {// Deviated right, but worse; want to go left
				turnLeft();
            }
        }
        
        if (*right_wheel_count > MAX_SPEED) {
            *right_wheel_count = MAX_SPEED;
        }
        if (*left_wheel_count > MAX_SPEED) {
            *left_wheel_count = MAX_SPEED;
        }
        if (*right_wheel_count < MIN_SPEED) {
            *right_wheel_count = MIN_SPEED;
        }
        if (*left_wheel_count < MIN_SPEED) {
            *left_wheel_count = MIN_SPEED;
        }
    }
}

void turnLeft() {
    isr_TS_Disable();
    int prevCountM1 = QuadDec_M1_GetCounter();
    int prevCountM2 = QuadDec_M2_GetCounter();
    PWM_1_WriteCompare(0);
    PWM_2_WriteCompare(TURN_SPEED);
    while (!sensor_readings[TOP_MID_SENSOR]) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
    }
    PWM_1_WriteCompare(TURN_SPEED);
    QuadDec_M1_SetCounter(prevCountM1);
    QuadDec_M2_SetCounter(prevCountM2);
    isr_TS_Enable();
}

void turnRight() {
    isr_TS_Disable();
    int prevCountM1 = QuadDec_M1_GetCounter();
    int prevCountM2 = QuadDec_M2_GetCounter();
    PWM_1_WriteCompare(TURN_SPEED);
    PWM_2_WriteCompare(0);
    while (!sensor_readings[TOP_MID_SENSOR]) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
    }
    PWM_2_WriteCompare(TURN_SPEED);
    QuadDec_M1_SetCounter(prevCountM1);
    QuadDec_M2_SetCounter(prevCountM2);
    isr_TS_Enable();
}

// --------------------------------------------- TURNS ------------------------------------------
void turnForDegrees(int degrees) {
    float distance_to_turn = degrees / 13;
    float distance_turned = 0;
    
    while (distance_turned < distance_to_turn) {
        // update sensor values so they're not 'stuck' when we leave function
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
        
         // New count values from encoder are ready
        if (timer_flag){
            isr_TS_Disable();
            
            distance_turned += getDistance(prevCountM1, prevCountM2);
            
            // Update previous values
            prevCountM1 = countM1;
            prevCountM2 = countM2;
            
            // Reset flag
            timer_flag = FALSE;
            
            isr_TS_Enable();
        }
    }
}

void sharpTurnLeft() {
    isr_TS_Disable();
    int prevCountM1 = QuadDec_M1_GetCounter();
    int prevCountM2 = QuadDec_M2_GetCounter();
    
    // go until we reach the junction
    while (!sensor_readings[BOTTOM_LEFT_SENSOR]) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
    }
    
    // make the turn
    setWheelDirection(FALSE, TRUE);
    PWM_1_WriteCompare(TURN_SPEED);
    PWM_2_WriteCompare(TURN_SPEED);
    QuadDec_M1_SetCounter(prevCountM1);
    QuadDec_M2_SetCounter(prevCountM2);
    isr_TS_Enable();
    
    turnForDegrees(60); // so that it doesn't stop if it's already on a line
    
    while (!sensor_readings[TOP_MID_SENSOR]) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
    }
    setWheelDirection(TRUE, TRUE);
}

void sharpTurnRight() {
    isr_TS_Disable();
    int prevCountM1 = QuadDec_M1_GetCounter();
    int prevCountM2 = QuadDec_M2_GetCounter();
    
    // go until we reach the junction
    while (!sensor_readings[BOTTOM_RIGHT_SENSOR]) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
    }
    
    // make the turn
    setWheelDirection(TRUE, FALSE);
    PWM_1_WriteCompare(TURN_SPEED);
    PWM_2_WriteCompare(TURN_SPEED);
    
    QuadDec_M1_SetCounter(prevCountM1);
    QuadDec_M2_SetCounter(prevCountM2);
    isr_TS_Enable();
    
    //45 may be too small.  Will try 60
    turnForDegrees(60); // so that it doesn't stop if it's already on a line
    
    while (!sensor_readings[TOP_MID_SENSOR]) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
    }
    setWheelDirection(TRUE, TRUE);
}

void goForwardWithoutTracking(int *left_wheel_count, int *right_wheel_count, int distance_to_travel) {
    // go forward slightly
    float distance = 0;
    while (distance < distance_to_travel) {
        
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
        
         // New count values from encoder are ready
        if (timer_flag){
            isr_TS_Disable();
            
            // update distance
            distance += getDistance(prevCountM1, prevCountM2);
            
            // Correct speed
            correctSpeed(prevCountM1,countM1,*left_wheel_count,TRUE);
            correctSpeed(prevCountM2,countM2,*right_wheel_count,FALSE);
            
            // Update previous values
            prevCountM1 = countM1;
            prevCountM2 = countM2;
            
            // Reset flag
            timer_flag = FALSE;
            
            isr_TS_Enable();
        }
    }
}
    
void uTurn(int *left_wheel_count, int *right_wheel_count) {
    goForwardWithoutTracking(left_wheel_count, right_wheel_count, SMALL_FORWARD_DISTANCE);
    
    isr_TS_Disable();
    int prevCountM1 = QuadDec_M1_GetCounter();
    int prevCountM2 = QuadDec_M2_GetCounter();
    
    // go until we reach the junction
    while (!sensor_readings[BOTTOM_RIGHT_SENSOR] && !sensor_readings[BOTTOM_LEFT_SENSOR] && sensor_readings[BOTTOM_MID_SENSOR]) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
    }
    
    // make the turn
    setWheelDirection(TRUE, FALSE);
    PWM_1_WriteCompare(TURN_SPEED);
    PWM_2_WriteCompare(TURN_SPEED);
    
    QuadDec_M1_SetCounter(prevCountM1);
    QuadDec_M2_SetCounter(prevCountM2);
    isr_TS_Enable();
    
    
    // the number 180 is roughly a 135 degree turn
    turnForDegrees(180); // so that it doesn't stop if it's already on a line
    
    // finish turn
    while (!sensor_readings[TOP_MID_SENSOR]) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
    }
    setWheelDirection(TRUE, TRUE);
}

// --------------------------------------------- HELPERS ------------------------------------------
void changeLeftWheelSpeed(int amount) {
    int new_value = PWM_1_ReadCompare() + amount;
    PWM_1_WriteCompare((new_value > 0) ? new_value:0); // account for underflow
}

void changeRightWheelSpeed(int amount){
    int new_value = PWM_2_ReadCompare() + amount;
    PWM_2_WriteCompare((new_value > 0) ? new_value:0); // account for underflow
}

void startWheels() {
    PWM_1_WriteCompare(300);
    PWM_2_WriteCompare(300);
}

void stopWheels() {
    PWM_1_WriteCompare(0);
    PWM_2_WriteCompare(0);
}

float getDistance(int prevCountM1, int prevCountM2) {
    float m1_dist = ((float)abs(countM1 - prevCountM1) / TICKS_PER_REV) * LINEAR_DISTANCE_PER_REV;
    float m2_dist = ((float)abs(countM2 - prevCountM2) / TICKS_PER_REV) * LINEAR_DISTANCE_PER_REV;
    return (m1_dist + m2_dist) / 2;
};

/* ## SPEED CONTROL ## 
* Gets difference between counts, compares to difference we expect. Add correction to PWM.*/
void correctSpeed(int prevCount, int count, int desired_count, int isLeftWheel){    
    // Account for overflow edge or other unknown error
    if (prevCount > count) return;
    
    // Account for when PSoc is powered but wheels aren't moving
    int diff_count = abs(count - prevCount);
    if (diff_count == 0) return;
        
    /* Determine correction that must be applied to the wheel
    * desired_count > diff_count :-> wheel is slow; increase PWM
    * desired_count < diff_count :-> wheel is fast; decrease PWM
    * desired_count == diff_count :-> wheel is stable; do nothing
    */
    int correction = desired_count-diff_count;
    
    // Apply the speed correction to the appropriate wheel          
    if (isLeftWheel) changeLeftWheelSpeed(correction);
    else changeRightWheelSpeed(correction);
    
    return;
}

void setWheelDirection(int leftIsForward, int rightIsForward){
    INV1_Write(!leftIsForward);
    INV2_Write(rightIsForward);
}

//* ========================================
void usbPutString(char *s) {
// !! Assumes that *s is a string with allocated space >=64 chars     
//  Since USB implementation retricts data packets to 64 chars, this function truncates the
//  length to 62 char (63rd char is a '!')
    
#ifdef PUTTY
    while (USBUART_CDCIsReady() == 0){ };
    USBUART_PutData((uint8 *)s, strlen(s));
#endif
    (void)s;
    return;
}

/* Prints the commands */
void printCommandsUSB(int8_t *commands){
    //fputs("Commands: (",stdout);
    usbPutString("Commands: (");
    int i;
    char buff[32];
    for (i = 0; i < MAX_COMMAND_LENGTH; i++){
        if (commands[i] == EMPTY_COMMAND){
            if (i > 0) break;
            return;
        }
        switch(commands[i]){
            case LEFT_TURN:
                usbPutString("L");
                break;
            case RIGHT_TURN:
                usbPutString("R");
                break;
            case U_TURN:
                usbPutString("U");
                break;
            default:
                itoa(commands[i], buff, 10);
                usbPutString(buff);
                break;
        };
        if (commands[i+1] != EMPTY_COMMAND) usbPutString(",");
    }
    usbPutString(")\r\n");
}

void printSingleCommandUSB(int8_t command){
    char buff[32];
    switch(command){
        case LEFT_TURN:
            usbPutString("L");
            break;
        case RIGHT_TURN:
            usbPutString("R");
            break;
        case U_TURN:
            usbPutString("U");
            break;
        default:
            itoa(command, buff, 10);
            usbPutString(buff);
            break;
    };
    usbPutString("\r\n");
}

/* Wrapper for BFS that gets a route connecting all food pellets in order */
void getRouteToFood(point start, point path[MAX_PATH_LENGTH]){
    
    #ifdef PUTTY
        usbPutString("Navigating food\r\n");
    #endif
    
    uint8_t i; // C90 is outdated smh

    /* Setup locations to use for BFS */
    point locations[6] = { 0 };
    locations[0] = (point){.x = start.x, .y = start.y};
    
    #ifdef PUTTY
        usbPutString("-- Locations set up\r\n");
    #endif

    /* Intialise path */
    memset(path, EMPTY_VAL, MAX_PATH_LENGTH*sizeof(point));

    /* Setup map for cleaning after BFS is done */
    int clean_map[MAP_ROW][MAP_COL] = { 0 };

    for (i = 0; i < MAP_ROW; i++){
        memcpy(clean_map[i], map[i], sizeof(int)*MAP_COL);
    }
    
    #ifdef PUTTY
        usbPutString("-- Clean map initialised\r\n");
    #endif
    
    uint8_t k; // Wishing it wasn't C90

    #ifdef PUTTY
        char buff[32];
    #endif
    
    for (i = 0; i < 5; i++){
        
        #ifdef PUTTY
            itoa(i,buff,10);
            usbPutString(buff);
            usbPutString("\r\n");
        #endif

        // Take next location from the food_list
        locations[i+1] = (point){.x = food_list[i][0], .y = food_list[i][1]};

        // Setup the path to store the BFS result
        point temp_path[MAX_PATH_LENGTH] = { 0 };
        temp_path[0] = (point){.x = EMPTY_VAL, .y = EMPTY_VAL};
        memset(temp_path, EMPTY_VAL, MAX_PATH_LENGTH*sizeof(point));

        // Big Fat Search
        BFS(locations[i],locations[i+1],temp_path);
        
        #ifdef PUTTY
            usbPutString("BFS Finished\r\n");
        #endif

        /* Clean up map from BFS */
        for (k = 0; k < MAP_ROW; k++){
            memcpy(map[k], clean_map[k], sizeof(int)*MAP_COL);
        }

        appendPath(path,len(path),temp_path);
    }

}

/* [] END OF FILE */
