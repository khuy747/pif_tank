/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "icm.h"
#include "motor.h"
#include "ibus.h"
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

/* USER CODE BEGIN PV */
//volatile uint32_t encoder_left_count = 0;
//volatile uint16_t encoder_right_count = 0;

//ICM_20602_t icm = {0};
//
//volatile HAL_StatusTypeDef icm_init_status = HAL_BUSY;
//volatile HAL_StatusTypeDef icm_read_status = HAL_BUSY;
//
//volatile uint32_t icm_read_count = 0;
//volatile uint32_t icm_error_count = 0;
//float yaw_deg = 0.0f;


extern iBUS_t ibus;
uint8_t frame[32];

uint8_t rx_byte;
volatile uint8_t last_rx_byte = 0;
volatile uint32_t rx_count = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static int32_t clamp_i32(int32_t x, int32_t min, int32_t max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

// Nhận tốc độ có dấu: -100 đến +100
static void motor_signed(int32_t left, int32_t right)
{
    left  = clamp_i32(left, -100, 100);
    right = clamp_i32(right, -100, 100);

    // Motor trái
    if (left >= 0)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
        motor_set_left((uint32_t)left);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
        motor_set_left((uint32_t)(100 + left));
    }

    // Motor phải
    if (right >= 0)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
        motor_set_right((uint32_t)right);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
        motor_set_right((uint32_t)(100 + right));
    }
}

static void car_control(uint16_t lv, uint16_t rv, uint16_t rh)
{
    // Chặn dữ liệu không hợp lệ
    if (lv < 900 || lv > 2100 ||
        rv < 900 || rv > 2100 ||
        rh < 900 || rh > 2100)
    {
        motor_signed(0, 0);
        return;
    }

    // 1. LV: ga 0 đến 100%
    int32_t throttle = clamp_i32(((int32_t)lv - 1000) / 10,
                                  0, 100);

    // 2. RV: -100 (lùi) đến +100 (tiến)
    int32_t direction = clamp_i32((1500 -(int32_t)rv) / 5,
                                   -100, 100);

    // 3. RH: -100 (trái) đến +100 (phải)
    int32_t steering = clamp_i32(( 1500 - (int32_t)rh) / 5,
                                  -100, 100);

    // Deadzone chống rung tại vị trí giữa
    if (rv >= 1475 && rv <= 1525)
        direction = 0;

    if (rh >= 1475 && rh <= 1525)
        steering = 0;

    // 4. Tính thành phần tiến/lùi và rẽ
    int32_t forward = throttle * direction / 100;
    int32_t turn    = throttle * steering / 100;

    // 5. Differential Drive
    int32_t left  = forward + turn;
    int32_t right = forward - turn;

    // 6. Chuẩn hóa để không vượt quá ga LV
    int32_t abs_left  = left < 0 ? -left : left;
    int32_t abs_right = right < 0 ? -right : right;

    int32_t max_abs = abs_left > abs_right ? abs_left : abs_right;

    if (max_abs > throttle && throttle > 0)
    {
        left  = left  * throttle / max_abs;
        right = right * throttle / max_abs;
    }

    // 7. Điều khiển motor
    motor_signed(left, right);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
//	float leftpwm = (float)(ibus.RH-1000)*99/1000+1;
//	float rightpwm = (float)(ibus.RV-1000)*99/1000+1;

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
  MX_TIM3_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */


//  icm_init(&hi2c1);
//
//  icm_calibrate_gyro(&hi2c1, &icm, 500);


  //Init PWM FOR DC Motor
  motor_init();

  //Init encoder

//  if (HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL) != HAL_OK) Error_Handler();
//
//  if (HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL) != HAL_OK) Error_Handler();

//  uint32_t last_tick = HAL_GetTick();
//  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0);
//  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
//Nếu GPIO là 0 và set pwm là 60 thì nó đi thẳng về trước tốc độ 60%. Còn nếu để pwm 60 và GPIO 1 thì nó sẽ đi lui với tốc độ 40% (Bù trừ nhau)
  //--> Cần viết thư viện để dựa vào tốc độ mình đi mà nó băm xung cho hợp lý.
  iBUS_init(&huart1);

  if (HAL_UART_Receive_IT(&huart1, &rx_byte, 1) != HAL_OK)
  {
      Error_Handler();
  }


//  iBUS_parse(frame, rx_byte);

//  iBUS_decode(&ibus, frame);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	  float leftpwm = (float)(ibus.RH-1000)*99/1000+1;
//	  float rightpwm = (float)(ibus.RV-1000)*99/1000+1;
//
//	  motor_set_left((uint32_t)(leftpwm));
//	  motor_set_right((uint32_t)(rightpwm));
//
//	  HAL_Delay(10);

	uint16_t lv, rv, rh;

	lv = ibus.LH;
	rv = ibus.RV;
	rh = ibus.RH;


	// Failsafe: không có frame hợp lệ trong 200 ms

	car_control(lv, rv, rh);

	HAL_Delay(10);

//	if (icm_readgyro(&hi2c1, &icm) == HAL_OK)
//	{
//		uint32_t now = HAL_GetTick();
//		float dt = (now - last_tick) / 1000.0f;
//		last_tick = now;
//
//		if (dt > 0.0f && dt < 0.5f)
//		{
//			yaw_deg += icm.gz_dps * dt;
//		}
//	}
//
//	HAL_Delay(100);



//	  encoder_left_count =
//	      __HAL_TIM_GET_COUNTER(&htim2);
//
//	  encoder_right_count =
//	      (uint16_t)__HAL_TIM_GET_COUNTER(&htim4);
//
//	  HAL_Delay(50);
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {

        uint8_t data = rx_byte;

        last_rx_byte = rx_byte;
        rx_count++;

        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);

        iBUS_parse(frame, data);

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
#ifdef USE_FULL_ASSERT
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
