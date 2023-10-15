#ifndef __MOTOR_H
#define __MOTOR_H

#include "main.h"
#include "tim.h"
#include "yqyPID.h"

typedef struct
{
  int32_t pos_mea;                  //测量位置
  int32_t spd_mea;                  //测量速度
  int16_t spd_set;
  uint32_t meaTime;                 //数据更新时间
  yqyPid_t pidSpd;
}motor_t;


extern motor_t motor[4];                             //电机ID  0,1,2,3

void Motor_Init(void);

void CalMotorMea(uint8_t id, TIM_HandleTypeDef *htim);
void PID_Cal_Handle(void);

void motorCtrl_RAW(uint8_t id, int16_t spd);       //id 0-3  速度±1000



#endif


