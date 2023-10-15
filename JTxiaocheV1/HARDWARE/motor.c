#include "motor.h"
#include "yqyPID.h"
#include "yqyPIDfodCtrl.h"
#include "tim.h"


#define MOTOR_CHANNEL_1_TOGGLE        0
#define MOTOR_CHANNEL_2_TOGGLE        0
#define MOTOR_CHANNEL_3_TOGGLE        0
#define MOTOR_CHANNEL_4_TOGGLE        0


motor_t motor[4];                             //电机ID  0,1,2,3

void Motor_Init(void)
{
  uint8_t i = 0;
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);
  
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim5, TIM_CHANNEL_ALL);
  
  TIM2->CNT = 0x7ff;
  TIM3->CNT = 0x7ff;
  TIM4->CNT = 0x7ff;
  TIM5->CNT = 0x7ff;
  
  HAL_TIM_Base_Start_IT(&htim6);
  
  for(i = 0; i < 4; i++)
  {
    motor[i].pidSpd.kp      = 5;
    motor[i].pidSpd.ki      = 0.25;
    motor[i].pidSpd.kd      = 0;
    motor[i].pidSpd.outMax  = 1000;
    motor[i].pidSpd.iMax    = 700;
    motor[i].pidSpd.iSep    = 0;
    motor[i].spd_set        = 0;
  }
}

static inline void CalMotorMea(uint8_t id, TIM_HandleTypeDef *htim)
{
  if(id > 3)return;
  motor[id].spd_mea = htim->Instance->CNT - 0x7ff;
  motor[id].pos_mea += motor[id].spd_mea;
  htim->Instance->CNT = 0x7ff;
  motor[id].meaTime = HAL_GetTick();
}

float TargetSpdTest = 0;

void PID_Cal_Handle(void)
{
  uint8_t i = 0;
  for(i = 0; i < 4; i++)
  {
    YQY_PID_Cal(&motor[i].pidSpd,motor[i].spd_mea,motor[i].spd_set);
    YQY_PID_FeedForwardCtrl(&motor[i].pidSpd);
    YQY_PID_ZeroForce(&motor[i].pidSpd);
    motorCtrl_RAW(i,motor[i].pidSpd.out);
  }
}

void motorCtrl_RAW(uint8_t id, int16_t spd)       //id 0-3  速度±1000
{
  switch(id)
  {
    case 0:
    {
      if(spd >= 0)
      {
        HAL_GPIO_WritePin(M1A_GPIO_Port,M1A_Pin,GPIO_PIN_SET);
        TIM8->CCR2 = 1000 - spd;
      }
      else
      {
        HAL_GPIO_WritePin(M1A_GPIO_Port,M1A_Pin,GPIO_PIN_RESET);
        TIM8->CCR2 = - spd;
      }
      break;
    }
    case 1:
    {
      if(spd >= 0)
      {
        HAL_GPIO_WritePin(M2A_GPIO_Port,M2A_Pin,GPIO_PIN_RESET);
        TIM8->CCR4 = spd;
      }
      else
      {
        HAL_GPIO_WritePin(M2A_GPIO_Port,M2A_Pin,GPIO_PIN_SET);
        TIM8->CCR4 = 1000 + spd;
      }
      break;
    }
    case 2:
    {
      if(spd >= 0)
      {
        HAL_GPIO_WritePin(M3A_GPIO_Port,M3A_Pin,GPIO_PIN_SET);
        TIM1->CCR1 = 1000 - spd;
      }
      else
      {
        HAL_GPIO_WritePin(M3A_GPIO_Port,M3A_Pin,GPIO_PIN_RESET);
        TIM1->CCR1 = - spd;
      }
      break;
    }
    case 3:
    {
      if(spd >= 0)
      {
        HAL_GPIO_WritePin(M4A_GPIO_Port,M4A_Pin,GPIO_PIN_RESET);
        TIM1->CCR3 = spd;
      }
      else
      {
        HAL_GPIO_WritePin(M4A_GPIO_Port,M4A_Pin,GPIO_PIN_SET);
        TIM1->CCR3 = 1000 + spd;
      }
      break;
    }
      
    default:break;
  }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance == TIM6)
  {
    CalMotorMea(0, &htim2);
    CalMotorMea(3, &htim3);
    CalMotorMea(1, &htim4);
    CalMotorMea(2, &htim5);
  }
}


