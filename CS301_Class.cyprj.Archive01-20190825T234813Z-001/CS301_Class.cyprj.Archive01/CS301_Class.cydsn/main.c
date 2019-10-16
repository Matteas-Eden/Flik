/* ========================================
 * Copyright Univ of Auckland, 2016
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Univ of Auckland.
 *
 * ================== INCLUDE ======================
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <project.h>

#include "defines.h"
#include "vars.h"
//* =================== MACROS =====================
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

// ADC stuff
#define ADC_READINGS_SIZE 90

// Speed calculations
#define LINEAR_DISTANCE_PER_REV 20.42035225
#define TICKS_PER_REV 228
#define TIMER_PERIOD 0.03

// Speed control
#define DESIRED_COUNT 8
#define TURN_SPEED 450
#define MIN_PWM_VAL 250

// Straight line test
#define CM_BETWEEN_COORDS 24

// 90 degree turn - TODO: Calculate true values
#define TICKS_FOR_TURN 190
#define TURN_PWM_VAL 225

// Line following macros - require calibration
#define ADJUST_SPEED_LARGE 3
#define ADJUST_SPEED_SMALL 1
#define MAX_SPEED 9
#define MIN_SPEED 4
#define SLOW_DOWN_SPEED 1

// Debug flag - uncomment when debugging
//#define PUTTY
#define PRINT_RATE 0x8000
//* ================= TPYE DEF =======================
//typedef struct wheel_speed{
//    float m1;
//    float m2;
//} speed;
typedef enum robot_state{
    CORRECT,
    SLIGHTLY_RIGHT,
    SLIGHTLY_LEFT,
    MEDIUM_RIGHT,
    MEDIUM_LEFT,
    TURN_RIGHT,
    TURN_LEFT,
    SLOW,
    PASS
} robot_state;
//* ================= FUNCTIONS =======================
void usbPutString(char *s);
int getMax(int * readings);

int32 getValForChannel(int16 n);
void updateSensorValues();

//speed getSpeed();
float getDistance(int prevCountM1, int prevCountM2);
void turnRight();
void turnLeft();
void sharpTurnRight(int *right_wheel_count, int *left_wheel_count);
void sharpTurnLeft(int *right_wheel_count, int *left_wheel_count);
int8 performTurn(int isLeft);

void goStraight(int desired_distance, int *right_speed_count, int *left_wheel_count);
void changeRightWheelSpeed(int amount);
void changeLeftWheelSpeed(int amount);
void invertWheels();
void setWheelDirection(int isLeftForward, int isRightForward);
void correctSpeed(int prevCount, int count, int desired_count, int isLeftWheel);

float resetDecoderCount(float sum);
//* ================= GLOBAL VARIABLES =======================
// ADC
int8 sensor_readings[NUM_SENSORS] = { 0 };
volatile int adc_flag = FALSE;
int adc_readings[NUM_SENSORS] = { 0 };
int sample_count = 0;
// Encoder
volatile int countM1 = 0;
volatile int countM2 = 0;
volatile int timer_flag = FALSE;
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
    // ========================================
    CYGlobalIntEnable;
    
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

    // ---------- USB SETUP ------------    
    #ifdef USE_USB    
        USBUART_Start(0, USBUART_5V_OPERATION);
    #endif            
    
    // To make sure wheels rotate in the same direction i.e. forward
    INV2_Write(1);
    
    // Storing count values
    int prevCountM1 = 0;
    int prevCountM2 = 0;
    int right_wheel_count = DESIRED_COUNT;
    int left_wheel_count = DESIRED_COUNT;
    int polling_count = 0;
    
    robot_state state;
    robot_state prev_state = CORRECT;
    
    //goStraight(CM_BETWEEN_COORDS, &right_wheel_count, &left_wheel_count);
    sharpTurnLeft(&right_wheel_count, &left_wheel_count);
    PWM_1_WriteCompare(0);
    PWM_2_WriteCompare(0);
    
//    while(1) { // Begin main control
//        polling_count++;
//        
//        if (adc_flag) {
//            updateSensorValues();
//            adc_flag = FALSE;
//        }
//        
//         // New count values from encoder are ready
//        if (timer_flag){
//            isr_TS_Disable();
//            
//            // Correct speed
//            correctSpeed(prevCountM1,countM1,left_wheel_count,TRUE);
//            correctSpeed(prevCountM2,countM2,right_wheel_count,FALSE);
//            
//            // Update previous values
//            prevCountM1 = countM1;
//            prevCountM2 = countM2;
//            
//            // Reset flag
//            timer_flag = FALSE;
//            
//            isr_TS_Enable();
//        }
//        
//       // follow a line algorithm
//        if (sensor_readings[TOP_MID_SENSOR]) { // on line
//            if (!sensor_readings[TOP_LEFT_SENSOR] && sensor_readings[TOP_RIGHT_SENSOR]) {// Deviated left; want to go right
//                state = SLIGHTLY_RIGHT;
//                right_wheel_count -= ADJUST_SPEED_SMALL;
//                left_wheel_count += ADJUST_SPEED_SMALL;
//            } else if (sensor_readings[TOP_LEFT_SENSOR] && !sensor_readings[TOP_RIGHT_SENSOR]) { // Deviated right; want to go left
//                state = SLIGHTLY_LEFT;
//                right_wheel_count += ADJUST_SPEED_SMALL;
//                left_wheel_count -= ADJUST_SPEED_SMALL;
//            } else { // Correct speed
//                state = CORRECT;
//            }
//        } else { // moved off
//            if (!sensor_readings[TOP_LEFT_SENSOR] && sensor_readings[TOP_RIGHT_SENSOR]) {// Deviated left, but worse; want to go right
//                state = MEDIUM_RIGHT;
//                turnRight();
//            } else if (sensor_readings[TOP_LEFT_SENSOR] && !sensor_readings[TOP_RIGHT_SENSOR]) {// Deviated right, but worse; want to go left
//                state = MEDIUM_LEFT;
//				turnLeft();
//            // for corners
//            } else if (!sensor_readings[TOP_LEFT_SENSOR] && !sensor_readings[TOP_RIGHT_SENSOR]) {
//                if (sensor_readings[BOTTOM_LEFT_SENSOR]) {
//                    state = TURN_LEFT;
//                    sharpTurnLeft();
//                } else if (sensor_readings[BOTTOM_RIGHT_SENSOR]) {
//                    state = TURN_RIGHT;
//                    sharpTurnRight();
//                 // slow down in preperation for a sharp turn
//                } else if (left_wheel_count > MIN_SPEED && right_wheel_count > MIN_SPEED) {
//                    state = SLOW;
//                    right_wheel_count -= SLOW_DOWN_SPEED;
//                    left_wheel_count -= SLOW_DOWN_SPEED;
//                } else {
//                    state = PASS;
//                }
//            }
//        }
//        
//        if (right_wheel_count > MAX_SPEED) {
//            right_wheel_count = MAX_SPEED;
//        }
//        if (left_wheel_count > MAX_SPEED) {
//            left_wheel_count = MAX_SPEED;
//        }
//        if (right_wheel_count < MIN_SPEED) {
//            right_wheel_count = MIN_SPEED;
//        }
//        if (left_wheel_count < MIN_SPEED) {
//            left_wheel_count = MIN_SPEED;
//        }
//        
////        if (polling_count == PRINT_RATE) {
//        if (prev_state != state) {
//            if (state == SLIGHTLY_LEFT) {
//                usbPutString("SLIGHTLY_LEFT\r\n");
//            } else if (state == SLIGHTLY_RIGHT) {
//                usbPutString("SLIGHTLY_RIGHT\r\n");
//            } else if (state == MEDIUM_LEFT) {
//                usbPutString("MEDIUM_LEFT\r\n");
//            } else if (state == MEDIUM_RIGHT) {
//                usbPutString("MEDIUM_RIGHT\r\n");
//            } else if (state == TURN_LEFT) {
//                usbPutString("TURN_LEFT\r\n");
//            } else if (state == TURN_RIGHT) {
//                usbPutString("TURN_RIGHT\r\n");
//            } else if (state == SLOW) {
//                usbPutString("SLOW\r\n");
//            } else if (state == PASS) {
//                usbPutString("PASS\r\n");
//            } else if (state == CORRECT) {
//                usbPutString("CORRECT\r\n");
//            }
//            prev_state = state;
//        }
//                
//                
//        if (polling_count == PRINT_RATE) polling_count = 0;
//    }
}

void goStraight(int desired_distance, int *right_wheel_count, int *left_wheel_count) {
    // Storing count values
    int prevCountM1 = 0;
    int prevCountM2 = 0;
//    int right_wheel_count = DESIRED_COUNT;
//    int left_wheel_count = DESIRED_COUNT;
    float distance = 0;
        
    while (distance < desired_distance) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
        
         // New count values from encoder are ready
        if (timer_flag){
            isr_TS_Disable();
            
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
            } else { // Correct speed
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

//char* itoa(int val, int base) {
//    static char buf[32] = {0};
//    
//    int i = 30;
//    for (; val && i ; --i, val /= base)
//        buf[i] = "01234566789abcdef"[val % base];
//        
//    return &buf[i+1];
//}

float getDistance(int prevCountM1, int prevCountM2){
    float m1_dist = ((float)abs(countM1 - prevCountM1) / TICKS_PER_REV) * LINEAR_DISTANCE_PER_REV;
    float m2_dist = ((float)abs(countM2 - prevCountM2) / TICKS_PER_REV) * LINEAR_DISTANCE_PER_REV;
    return (m1_dist+m2_dist)/2;
};

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

void changeLeftWheelSpeed(int amount) {
    int new_value = PWM_1_ReadCompare() + amount;
    PWM_1_WriteCompare((new_value > 0) ? new_value:0); // account for underflow
}

void changeRightWheelSpeed(int amount){
    int new_value = PWM_2_ReadCompare() + amount;
    PWM_2_WriteCompare((new_value > 0) ? new_value:0); // account for underflow
}

/* ## SPEED CONTROL ## 
* Algorithm: Get difference between counts, compare to
* the difference we expect. Add the correction to the PWM.
*/
void correctSpeed(int prevCount, int count, int desired_count, int isLeftWheel){    
    
    // Account for overflow edge or other unknown error
    if (prevCount > count) return;
    
    int diff_count = abs(count - prevCount);
    
    // Account for a situation where PSoc is powered but wheels aren't moving
    if (diff_count == 0) return;
    
    char msg[64];
        
    /* Determine correction that must be applied to the wheel
    * desired_count > diff_count :-> wheel is slow; increase PWM
    * desired_count < diff_count :-> wheel is fast; decrease PWM
    * desired_count == diff_count :-> wheel is stable; do nothing
    */
    int correction = desired_count-diff_count;
    
    // Apply the speed correction to the appropriate wheel          
    if (isLeftWheel){
        changeLeftWheelSpeed(correction);
    }
    else{
        changeRightWheelSpeed(correction);
    }
    
    return;
}

/* Set wheel direction explicitly
* Parameters are boolean, UNKNOWN can be passed
* in if you wish the value to be unchanged
*/
void setWheelDirection(int leftIsForward, int rightIsForward){
    INV1_Write(!leftIsForward);
    INV2_Write(rightIsForward);
}

void turnLeft() {
    PWM_1_WriteCompare(0);
    PWM_2_WriteCompare(TURN_SPEED);
    while (!sensor_readings[TOP_MID_SENSOR]) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
    }
    PWM_1_WriteCompare(TURN_SPEED);
}

void turnRight() {
    PWM_1_WriteCompare(TURN_SPEED);
    PWM_2_WriteCompare(0);
    while (!sensor_readings[TOP_MID_SENSOR]) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
    }
    PWM_2_WriteCompare(TURN_SPEED);
}

void sharpTurnLeft(int *right_wheel_count, int *left_wheel_count) {
    int prevCountM1 = 0;
    int prevCountM2 = 0;
    
    // go until we reach the line
    while (!sensor_readings[BOTTOM_LEFT_SENSOR]) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
        
         // New count values from encoder are ready
        if (timer_flag){
            isr_TS_Disable();
            
            // Correct speed
            correctSpeed(prevCountM1,countM1,*left_wheel_count,TRUE);
            correctSpeed(prevCountM2,countM2,*right_wheel_count,FALSE);
            
            *left_wheel_count -= SLOW_DOWN_SPEED;
            *right_wheel_count -= SLOW_DOWN_SPEED;
            if (*right_wheel_count < MIN_SPEED) {
                *right_wheel_count = MIN_SPEED;
            }
            if (*left_wheel_count < MIN_SPEED) {
                *left_wheel_count = MIN_SPEED;
            }
            
            // Update previous values
            prevCountM1 = countM1;
            prevCountM2 = countM2;
            
            // Reset flag
            timer_flag = FALSE;
            
            isr_TS_Enable();
        }
        
    }
    
    // make the turn
    setWheelDirection(FALSE, TRUE);
    PWM_1_WriteCompare(TURN_SPEED);
    PWM_2_WriteCompare(TURN_SPEED);
    while (!sensor_readings[TOP_MID_SENSOR]) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
    }
    setWheelDirection(TRUE, TRUE);
    PWM_1_WriteCompare(TURN_SPEED);
    PWM_2_WriteCompare(TURN_SPEED);
}

void sharpTurnRight(int *right_wheel_count, int *left_wheel_count) {
    int prevCountM1 = 0;
    int prevCountM2 = 0;
    
    // go until we reach the line
    while (!sensor_readings[BOTTOM_RIGHT_SENSOR]) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
        
         // New count values from encoder are ready
        if (timer_flag){
            isr_TS_Disable();
            
            // Correct speed
            correctSpeed(prevCountM1,countM1,*left_wheel_count,TRUE);
            correctSpeed(prevCountM2,countM2,*right_wheel_count,FALSE);
            
            *left_wheel_count -= SLOW_DOWN_SPEED;
            *right_wheel_count -= SLOW_DOWN_SPEED;
            if (*right_wheel_count < MIN_SPEED) {
                *right_wheel_count = MIN_SPEED;
            }
            if (*left_wheel_count < MIN_SPEED) {
                *left_wheel_count = MIN_SPEED;
            }
            
            // Update previous values
            prevCountM1 = countM1;
            prevCountM2 = countM2;
            
            // Reset flag
            timer_flag = FALSE;
            
            isr_TS_Enable();
        }
    }
    
    // make the turn
    setWheelDirection(TRUE, FALSE);
    PWM_1_WriteCompare(TURN_SPEED);
    PWM_2_WriteCompare(TURN_SPEED);
    while (!sensor_readings[TOP_MID_SENSOR]) {
        if (adc_flag) {
            updateSensorValues();
            adc_flag = FALSE;
        }
    }
    setWheelDirection(TRUE, TRUE);
    PWM_1_WriteCompare(TURN_SPEED);
    PWM_2_WriteCompare(TURN_SPEED);
}
//* ================== UNUSED FUNCTIONS ======================

/*
* Function for performing a 90 degree turn
* returns true once finished
*/
//int8 performTurn(int isLeft){
//    // Reset decoder measurements
//    QuadDec_M1_SetCounter(0);
//    QuadDec_M2_SetCounter(0);
//    
//    int M1_count, M2_count = 0;
//    
//    // Set PWMs to rotate
//    PWM_1_WriteCompare(TURN_PWM_VAL);
//    PWM_2_WriteCompare(TURN_PWM_VAL);
//    
//    // Set wheel direction for rotating on the spot
//    if (isLeft){
//        // left wheel moves backwarsd, right moves forwards
//        setWheelDirection(FALSE, TRUE);}
//    else {
//        // right wheel moves forwards, left moves backwards
//        setWheelDirection(TRUE, FALSE);
//    }
//    
//    while (M1_count < TICKS_FOR_TURN || M2_count < TICKS_FOR_TURN) {
//        M1_count = QuadDec_M1_GetCounter();
//        M2_count = QuadDec_M2_GetCounter();
//    }
//    
//    PWM_1_WriteCompare(0);
//    PWM_2_WriteCompare(0);
//    
//    return TRUE;
//}


//* ========================================
void usbPutString(char *s)
{
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

/* [] END OF FILE */
