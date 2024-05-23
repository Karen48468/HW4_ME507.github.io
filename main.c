/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "motor_driver.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
 * @brief specifies the timer used for one of the motors
 */
TIM_HandleTypeDef htim2;

/**
 * @brief specifies the timer used for one of the motors
 */
TIM_HandleTypeDef htim3;

/**
 * @brief used for callback function and to communicate with device
 */
UART_HandleTypeDef huart2;


/* USER CODE BEGIN PV */
// Define variables used here
/**
 * @brief stores the values from the user
 */
char char_in[5];

/**
 * @brief variable to specify what state it is in and when it is appropriate to move on to the next state
 */
int state = 0;

/**
 * @brief sets appropriate pulse width for the motor
 */
int pw;

/**
 * @brief specifies which motor is being used
 */
static uint8_t motor = 0;

/**
 * @brief checks that the first character is an M to specify motor
 */
static uint8_t FIRST;

/**
 * @brief checks that the second character is a 1 or 2, which specifies which motor is being used
 */
static uint8_t SECOND;

/**
 * @brief defines the hexadecimal value for the duty cycle for the motor
 */
char HEX[2];

/**
 * @brief keeps track of the input of characters from keyboard
 */
static uint8_t count = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */

  // Make Motor 1 Objects
  Motor_Driver MOT1 ={.htim = &htim2,
  	  .channel_1 = TIM_CHANNEL_1,
  	  .channel_2 = TIM_CHANNEL_2};

  // Make Motor 2 Objects
  Motor_Driver MOT2 = {.htim = &htim3,
  	  .channel_1 = TIM_CHANNEL_1,
	  .channel_2 = TIM_CHANNEL_2};

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // Enable ECHO
	  HAL_UART_Receive_IT(&huart2, (uint8_t*)&char_in, 1);

	  if (state == 0){
		  // Placeholder to go back to callback function
		  motor = 0;

	  }

	  // ---------------[STATE 1:Check Motor]---------------
	  if (state == 1){
		  // Check that only 4 char. were entered
		  if (FIRST == 'M'){

			  // Check which motor was chosen and set flag
			  if (SECOND == '1'){
				  motor = 1;
				  enable_motor(&MOT1);
				  state = 2;
			  }

			  else if (SECOND == '2'){
				  motor = 2;
				  enable_motor(&MOT2);
				  state = 2;
			  }

			  // Any other inputs are "invalid"
			  else{
				  HAL_UART_Transmit(&huart2,(uint8_t*)"\r\nInvalid Motor Input \r\n", strlen("\r\nInvalid Motor Input \r\n"), HAL_MAX_DELAY);
				  HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", strlen("\r\n"), HAL_MAX_DELAY);
				  state = 0;
			  }
		  }
		  else{
			  HAL_UART_Transmit(&huart2,(uint8_t*)"\r\nInvalid Input \r\n", strlen("\r\nInvalid Input \r\n"), HAL_MAX_DELAY);
			  HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", strlen("\r\n"), HAL_MAX_DELAY);
			  state = 0;
		  }
	  }

	  // ---------------[STATE 2: Check Hex]---------------
	  else if (state == 2){
		  state = 0;
		  // Convert hex to decimal with strtol, Hex specified at end with 16
		  long int pulse = strtol(HEX, NULL, 16);

		  // Check for negative hexadecimal and convert appropriately
		  if (pulse >= 128){
			  pulse ^= 128;
			  pulse -= 128;
		  }

		  // Conversion factor for duty_cycle
		  pw = pulse * 3.125;

		  if (pw<=400 && pw>=-400){

			  // If the pulse is zero, disable the corresponding motor
			  if (pulse == 0){
				  if (motor == 1){
					  disable_motor(&MOT1);
					  HAL_UART_Transmit(&huart2,(uint8_t*)"\r\nYou disabled Motor 1 \r\n", strlen("\r\nYou disabled Motor 1 \r\n"), HAL_MAX_DELAY);
					  motor = 0;
				  }

				  else if (motor == 2){
					  disable_motor(&MOT2);
					  HAL_UART_Transmit(&huart2,(uint8_t*)"\r\nYou disabled Motor 2 \r\n", strlen("\r\nYou disabled Motor 2 \r\n"), HAL_MAX_DELAY);
					  motor = 0;
				  }

			  }

			  // Otherwise set the corresponding duty cycle for the corresponding motor
			  else if (motor == 1){
				  set_duty(&MOT1, pw);
				  HAL_UART_Transmit(&huart2,(uint8_t*)"\r\nValid input for Motor 1 \r\n", strlen("\r\nValid input for Motor 1 \r\n"), HAL_MAX_DELAY);
				  HAL_Delay(100);
				  pw = 0;
			  }

			  else if (motor == 2){
				  set_duty(&MOT2, pw);
				  HAL_UART_Transmit(&huart2,(uint8_t*)"\r\nValid input for Motor 2 \r\n", strlen("\r\nValid input for Motor 2 \r\n"), HAL_MAX_DELAY);
				  HAL_Delay(100);
				  pw = 0;
			  }
		  }

		  else{
			  HAL_UART_Transmit(&huart2,(uint8_t*)"\r\nInvalid duty cycle \r\n", strlen("\r\nInvalid duty cycle \r\n"), HAL_MAX_DELAY);
		  }
		  HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n", strlen("\r\n"), HAL_MAX_DELAY);
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 9;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 399;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 160;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 9;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 399;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 160;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

// Code to ECHO characters
void HAL_UART_RxCpltCallback (UART_HandleTypeDef* huart)
{
	/**
	 * @brief
	 * A function that handles a user's keyboard input and "echos" the result.
	 * No more than five characters are allowed.
	 * @param count  defines how many characters are input
	 * @param char_in stores the values from the user
	 */

	// Transmit displays the character to Putty
	HAL_UART_Transmit(&huart2, (uint8_t*)&char_in[count], 1, HAL_MAX_DELAY);
	count++;

	// Ensure there is only an input of 4
	if (count < 4){
		HAL_UART_Receive_IT(&huart2, (uint8_t*)&char_in[count], 1);
		// use count to 'append' variable

	}

	// Else move on to state 1
	else{
		FIRST = char_in[0];
		SECOND = char_in[1];
		HEX[0] = char_in[2];
		HEX[1] = char_in[3];

		// Clear the char_in array
		memset(char_in, 0, sizeof(char_in));

		state = 1;
		count = 0;
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
