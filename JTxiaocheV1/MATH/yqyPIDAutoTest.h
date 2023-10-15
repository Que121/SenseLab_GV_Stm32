#ifndef __YQY_PID_AUTO_SET_H

#define __YQY_PID_AUTO_SET_H


#include "main.h"
#include "yqyPID.h"

typedef struct
{
  float testSpeed;          //�Զ����ģ�����
  float errDelta;
  float PIDlastError;
}yqyAutoTest_t;

extern yqyAutoTest_t yqyAutoTest;


void YQY_PID_AutoTest_OneMotor(uint8_t id);
void MotorCtrl_Handle(uint8_t id);              //��id�������PID����������
void AppLogData(int32_t data[4][11]);

#endif

