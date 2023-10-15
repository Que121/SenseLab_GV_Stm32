#ifndef __YQY_PID_AUTO_SET_H

#define __YQY_PID_AUTO_SET_H


#include "main.h"
#include "yqyPID.h"

typedef struct
{
  float testSpeed;          //自动更改，请勿动
  float errDelta;
  float PIDlastError;
}yqyAutoTest_t;

extern yqyAutoTest_t yqyAutoTest;


void YQY_PID_AutoTest_OneMotor(uint8_t id);
void MotorCtrl_Handle(uint8_t id);              //对id电机进行PID控制与运算
void AppLogData(int32_t data[4][11]);

#endif

