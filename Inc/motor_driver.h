// motor_driver
// H file used to define function and variables
// A function to set the duty cycle of one of the motor driver channels
// A function to enable one of the motor driver channels
// A function to disable one of the motor driver channels

#ifndef _MOTOR_DRIVER_H_
#define _MOTOR_DRIVER_H_

#include <stdio.h>
#include <stdint.h>
#include "stm32l4xx_hal.h"

// Motor object data structure
// Create all the variables needed for the function in a struct
typedef struct {
	//int motor;                  // specify the motor you are using?
    //int32_t  PWM;               // specify the duty cycle for the chosen motor
	TIM_HandleTypeDef *htim;    // specify the timer for the motor
    uint32_t channel_1;         // specify the channel for the motor
    uint32_t channel_2;         // specify the channel for the motor

} Motor_Driver;

// Prototype for motor object "method" - setting the function
// p_mot is a motor pointer; pointers are variables that holds address of another variable
void enable_motor(Motor_Driver *motor);

void disable_motor(Motor_Driver *motor);

void set_duty(Motor_Driver *motor, int32_t PWM);

#endif /* _MOTOR_DRIVER_H_ */
