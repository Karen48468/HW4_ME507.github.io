#include "motor_driver.h"

/**
 * @author Karen Morales De Leon
 * @date   APril 19, 2024
 * @brief
 * Motor Driver can enable and disable the desired motor. The duty cycle can also be specified once motor is enabled.
 */

#define Max_PWM 400
/**
 * @brief
 * Parameter sets the maximum allowable for the duty cycle for the specified motor.
 */

void enable_motor(Motor_Driver *motor) {
	/**
	 * @brief
	 * A function that sets the duty cycle of the specified motor
	 * The function saturates the duty cycle such that it is not set greater than 100%
	 * @param motor  defines the timer and channel that specifies which motor it is using
	 */

	HAL_TIM_PWM_Start(motor->htim, motor->channel_1);
	HAL_TIM_PWM_Start(motor->htim, motor->channel_2);
}

void disable_motor(Motor_Driver *motor) {
	/**
	 * @brief
	 * A function that sets the duty cycle of the specified motor
	 * The function saturates the duty cycle such that it is not set greater than 100%
	 * @param motor  defines the timer and channel that specifies which motor it is using
	 */

	HAL_TIM_PWM_Stop(motor->htim, motor->channel_1);
	HAL_TIM_PWM_Stop(motor->htim, motor->channel_2);
}

void set_duty(Motor_Driver *motor, int32_t PWM) {
	/**
	 * @brief
	 * A function that sets the duty cycle of the specified motor
	 * The function saturates the duty cycle such that it is not set greater than 100%
	 * @param motor  defines the timer and channel that specifies which motor it is using
	 */

	// Saturation Block
	if (PWM>Max_PWM){
		PWM = 399;
	}

	else if (PWM<-Max_PWM){
		PWM = -399;
	}

	// Clockwise
	else if (PWM>0){
		__HAL_TIM_SET_COMPARE(motor->htim, motor->channel_1, PWM);
		__HAL_TIM_SET_COMPARE(motor->htim, motor->channel_2, 0);
	}

	// Counter Clockwise
	else if (PWM<0){
		__HAL_TIM_SET_COMPARE(motor->htim, motor->channel_1, 0);
		__HAL_TIM_SET_COMPARE(motor->htim, motor->channel_2, -PWM);
	}
}


