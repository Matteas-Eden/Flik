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
#define BFS_ROUTE_SIZE 68
#define DISTANCE_THRESHOLD 20

// Debug flag - uncomment when debugging
//#define PUTTY
//* ================= FUNCTIONS =======================
void usbPutString(char *s);

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

// turning
void turnForDegrees(int degrees);
void sharpTurnRight();
void sharpTurnLeft();
void uTurn(int *left_wheel_count, int *right_wheel_count);

// debugging
void printCommandsUSB(int *commands);
void printSingleCommandUSB(int command);
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
    // Change these for pathing
//    point start = {.x=3, .y=13};//17,5; 1,1; 3,13
//    point destination = {.x = 11, .y = 13};//3,13; 17,13;11,13
//    point route[BFS_ROUTE_SIZE];        //facing up; right; 
//
//    int i;
//    for (i = 0; i < BFS_ROUTE_SIZE; i++) {
//        route[i] = (point){.x=-1, .y=-1};
//    }
//
//    BFS(start, destination, route);
//
//    int directions[MAX_COMMAND_LENGTH];
//    convertCoordinatesToCommands(route, directions);
    
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
    
    int direction_index = 0;
    int directions[30] = {2, RIGHT_TURN, 2, RIGHT_TURN, 4, RIGHT_TURN,2, LEFT_TURN, 4, LEFT_TURN, 2, LEFT_TURN, 2 , RIGHT_TURN, 4, U_TURN, 2, LEFT_TURN, 4, RIGHT_TURN, 2, LEFT_TURN, 2 , RIGHT_TURN, 2 , LEFT_TURN, 4 };
    //int directions[2] = {RIGHT_TURN, EMPTY_COMMAND};
    
    usbPutString("## Testing Algorithm ##\r\n");
    printCommandsUSB(directions);
        
    while (directions[direction_index] != 0) {
        startWheels();
        
        if (directions[direction_index] > DISTANCE_THRESHOLD) break;
        
        if (directions[direction_index] == RIGHT_TURN) {
            usbPutString("Sharp turn right\r\n");
            sharpTurnRight(&right_wheel_count, &left_wheel_count);
        } else if (directions[direction_index] == LEFT_TURN) {
            usbPutString("Sharp turn left\r\n");
            sharpTurnLeft(&right_wheel_count, &left_wheel_count);
        } else if (directions[direction_index] == U_TURN) {
            usbPutString("U turn\r\n");
            uTurn(&right_wheel_count, &left_wheel_count);
        } else if (directions[direction_index] <= DISTANCE_THRESHOLD){
            usbPutString("Straight\r\n");
            // Change this for int array
            //int num_coords = directions[direction_index] - '0';
            int num_coords = directions[direction_index];
            goStraightForBlock(num_coords, &right_wheel_count, &left_wheel_count);
        }
        stopWheels();
        printSingleCommandUSB(directions[direction_index]);
        CyDelay(1000);
        
//        int num_coords;
//            
//        switch(directions[direction_index]) {
//            case 'R':
//                sharpTurnRight(&right_wheel_count, &left_wheel_count);
//                usbPutString("Sharp turn right\r\n");
//                break;
//            case 'L':
//                sharpTurnLeft(&right_wheel_count, &left_wheel_count);
//                usbPutString("Sharp turn left\r\n");
//                break;
//            case 'U':
//                uTurn(&right_wheel_count, &left_wheel_count);
//                usbPutString("U turn\r\n");
//                break;
//            default: 
//                usbPutString("Straight\r\n");
//                num_coords = directions[direction_index] - '0';
//                goStraightForBlock(num_coords, &right_wheel_count, &left_wheel_count);
//                break;
//        }
        direction_index++;
    }
    
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
            itoa(adc_readings[i], buff, 10);
            usbPutString(buff);
            usbPutString(" ");
            sensor_readings[i] = !!(adc_readings[i] < LIGHT_LEVEL);
            adc_readings[i] = 0; // Reset max value from samples
        }
        
        usbPutString("\r\n");
        
        ADC_DEBUG_Write(1);
        sample_count = 0;
    }
    else ADC_DEBUG_Write(0);
    
}

// --------------------------------------------- STRAIGHT ------------------------------------------
void goStraightForBlock(int desired_blocks, int *right_wheel_count, int *left_wheel_count) {
//    usbPutString("\r\n");
//    usbPutString(" - desired  ");
    
    *right_wheel_count = DESIRED_COUNT;//TODO: check if needed
    *left_wheel_count = DESIRED_COUNT;
    
    float distance = 0;
    int desired_distance = GRID_SIZE * desired_blocks - HALF_ROBOT_LENGTH;
    
//    char buf[32];
//    itoa(desired_distance, buf, 10);
//    usbPutString(buf);
//    usbPutString("\r\n - distance ");
//    itoa(distance, buf, 10);
//    usbPutString(buf);
//    usbPutString("\r\n");
//    usbPutString("\r\n");
    
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
            
//            usbPutString("\r\n - desired - ");
//            itoa(desired_distance, buf, 10);
//            usbPutString(buf);
//            usbPutString(" - distance - ");
//            itoa(distance, buf, 10);
//            usbPutString(buf);
            
            
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
                usbPutString(" - turn right\r\n");
                turnRight();
            } else if (sensor_readings[TOP_LEFT_SENSOR] && !sensor_readings[TOP_RIGHT_SENSOR]) {// Deviated right, but worse; want to go left
                usbPutString(" - turn left\r\n");
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
    usbPutString(" - forward\r\n");
    // go until we reach the junction
    while (!sensor_readings[BOTTOM_LEFT_SENSOR]) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
    }
    
    usbPutString(" - turn 45\r\n");
    
    // make the turn
    setWheelDirection(FALSE, TRUE);
    PWM_1_WriteCompare(TURN_SPEED);
    PWM_2_WriteCompare(TURN_SPEED);
    QuadDec_M1_SetCounter(prevCountM1);
    QuadDec_M2_SetCounter(prevCountM2);
    isr_TS_Enable();
    
    turnForDegrees(55); // so that it doesn't stop if it's already on a line
    
    usbPutString(" - finish turn\r\n");
    
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
    usbPutString("---------------------------\r\n");
    usbPutString("-------- forward -----------\r\n");
    usbPutString("---------------------------\r\n");
    
    // go until we reach the junction
    while (!sensor_readings[BOTTOM_RIGHT_SENSOR]) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
    }
    
    usbPutString("---------------------------\r\n");
    usbPutString("-------- turn 45 ----------\r\n");
    usbPutString("---------------------------\r\n");
//    stopWheels();
//    CyDelay(1000);
//    startWheels();
    
    // make the turn
    setWheelDirection(TRUE, FALSE);
    PWM_1_WriteCompare(TURN_SPEED);
    PWM_2_WriteCompare(TURN_SPEED);
    
    QuadDec_M1_SetCounter(prevCountM1);
    QuadDec_M2_SetCounter(prevCountM2);
    isr_TS_Enable();
    
    //45 may be too small.  Will try 55
    turnForDegrees(55); // so that it doesn't stop if it's already on a line
    
    usbPutString("---------------------------\r\n");
    usbPutString("-------- finish turn ------\r\n");
    usbPutString("---------------------------\r\n");
//    stopWheels();
//    CyDelay(1000);
//    startWheels();
    
    while (!sensor_readings[TOP_MID_SENSOR]) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
    }
    setWheelDirection(TRUE, TRUE);
}
    
void uTurn(int *left_wheel_count, int *right_wheel_count) {
    usbPutString("U turn\r\n");
    
    usbPutString(" - forward slightly\r\n");
    // go forward slightly
    float distance = 0;
    while (distance < SMALL_FORWARD_DISTANCE) {
        
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
    
    isr_TS_Disable();
    int prevCountM1 = QuadDec_M1_GetCounter();
    int prevCountM2 = QuadDec_M2_GetCounter();
    
    usbPutString(" - forward till junction\r\n");
    // go until we reach the junction
    while (!sensor_readings[BOTTOM_RIGHT_SENSOR] && !sensor_readings[BOTTOM_LEFT_SENSOR] && sensor_readings[BOTTOM_MID_SENSOR]) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
    }
    
    usbPutString(" - turn 135\r\n");
    
    // make the turn
    setWheelDirection(TRUE, FALSE);
    PWM_1_WriteCompare(TURN_SPEED);
    PWM_2_WriteCompare(TURN_SPEED);
    
    QuadDec_M1_SetCounter(prevCountM1);
    QuadDec_M2_SetCounter(prevCountM2);
    isr_TS_Enable();
    
    
    // the number 180 is roughly a 135 degree turn
    turnForDegrees(180); // so that it doesn't stop if it's already on a line
    
    usbPutString(" - finish turn\r\n");
    
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
    PWM_1_WriteCompare(400);
    PWM_2_WriteCompare(400);
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
void printCommandsUSB(int *commands){
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

void printSingleCommandUSB(int command){
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

/* [] END OF FILE */
