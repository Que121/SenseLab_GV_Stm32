/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "motor.h"
#include "yqyPIDAutoTest.h"
#include "flash.h"
#include "HYQY_RECV.h"
#include "DR16.h"
#include "math.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int tempCtrlSpd = 0;
extern int32_t MotorValLab[4][11];
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  int16_t speed_X,speed_Y;
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
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  MX_USART1_UART_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  Motor_Init();
  printf("hello YQY\r\n");
  
  DR16_Init();
  
//  TestFlash();
  
  HAL_Delay(100);
  
  ReadDataFromLastPage((uint32_t*)MotorValLab, 44);               //默认读取该校准系数。请勿随意更改。
//  AppLogData(MotorValLab);
  HYQY_Recv_Init(&HYQY_recvInfo, 0);
  HAL_Delay(1000);
  
  if(HAL_GPIO_ReadPin(KEY_GPIO_Port,KEY_Pin) == GPIO_PIN_RESET)   //按下按键后开机，进行校准
  {
    YQY_PID_AutoTest_OneMotor(2);           
    YQY_PID_AutoTest_OneMotor(3);
    YQY_PID_AutoTest_OneMotor(1);
    YQY_PID_AutoTest_OneMotor(0);
  }
  
  //while(HAL_GPIO_ReadPin(KEY_GPIO_Port,KEY_Pin) == GPIO_PIN_SET);
  
  printf("startRun\r\n");
  
  HAL_Delay(1000);
  //YQY_PID_AutoTest_OneMotor(0);
  
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if(IsReceiveDataOK(&HYQY_recvInfo))       //进行接收测试：如果接收到数据
    {
      ShowDecodeResult(&HYQY_recvInfo);       //打印出数据
      
      switch(HYQY_recvInfo.recvRes[0])
      {
        case 1:                               //操作1，直接电机速度PID控制
        {
          motor[0].spd_set = HYQY_recvInfo.recvRes[1] - 100;
          motor[1].spd_set = HYQY_recvInfo.recvRes[2] - 100;
          motor[2].spd_set = HYQY_recvInfo.recvRes[3] - 100;
          motor[3].spd_set = HYQY_recvInfo.recvRes[4] - 100;
          printf("Motor Spd Update\r\n");
          break;
        }
        case 2:                               //操作2，电机关闭
        {
          motor[0].spd_set = 0;
          motor[1].spd_set = 0;
          motor[2].spd_set = 0;
          motor[3].spd_set = 0;
          printf("Motor Stop\r\n");
          break;
        }
        case 3:                               //操作3，按前进速度和旋转速度走
        {
          speed_X = HYQY_recvInfo.recvRes[1] - 100;
          speed_Y = HYQY_recvInfo.recvRes[2] - 100;
          
          motor[0].spd_set =  speed_X + speed_Y;
          motor[1].spd_set =  speed_X - speed_Y;
          motor[2].spd_set = -speed_X + speed_Y;
          motor[3].spd_set = -speed_X - speed_Y;
          
          printf("Motor Stop\r\n");
          break;
        }
        
        default:
        {
          printf("Error Options For Code:%d\r\n", HYQY_recvInfo.recvRes[0]);
          break;
        }
      }
    }
    
    if(HAL_GetTick() - remote_control.LastTime < 1000)
    {
      if((remote_control.ch4 <= 660) && (remote_control.ch4 >= -660))
      {
        if((remote_control.ch4 <= 30) && (remote_control.ch4 >= -30))speed_X = 0;
        else  speed_X = remote_control.ch4 / 7.0;
        
        if((remote_control.ch3 <= 30) && (remote_control.ch3 >= -30))speed_Y = 0;
        else  speed_Y = remote_control.ch3 / 7.0;
        
        motor[0].spd_set =  speed_X + speed_Y;
        motor[1].spd_set =  speed_X - speed_Y;
        motor[2].spd_set = -speed_X + speed_Y;
        motor[3].spd_set = -speed_X - speed_Y;
        
      }
    }
    else if(HAL_GetTick() - remote_control.LastTime < 1200)
    {
      motor[0].spd_set = 0;
      motor[1].spd_set = 0;
      motor[2].spd_set = 0;
      motor[3].spd_set = 0;
    }
    
    
    
    PID_Cal_Handle();
    HAL_Delay(50);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
extern uint8_t UartRecvBuff;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *husart)
{
  if(husart->Instance == USART1)                    //如果串口1接收到数据
  {
    HYQY_Recv_Handle(&HYQY_recvInfo,UartRecvBuff);  //调用该句柄进行解码
    HAL_UART_Receive_IT(&huart1,&UartRecvBuff,1);   //继续进行接收
  }
  /* 判断是哪个串口触发的中断 */
	if(husart ->Instance == USART2)										//遥控器发回来的数据
	{
		DR16_callBackHandle();
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
