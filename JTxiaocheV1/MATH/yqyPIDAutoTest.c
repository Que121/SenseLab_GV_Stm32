#include "yqyPIDAutoTest.h"

#include "main.h"
#include "stdio.h"
#include "math.h"
#include "flash.h"

#define USE_PRINTF          1                     //����printf����
#define STEDY_ELAPS_TIME    2000                  //ÿ�������ȶ�3��
#define STEDY_ERROR_DEATH   4.5                   //���С�ڸ÷�Χ����ʾ����
#define PID_CALCULATE_ELAPS 50                    //PIDÿ50ms����һ��
#define SETDY_WAIT_TIME_MAX 4000                  //���8000ms�ڣ�errû���������ж����ٶ�̫���ˣ��޷��ﵽ��������һУ׼����


yqyAutoTest_t yqyAutoTest;

int32_t MotorValTestLab[4][11] =          //20,40,60,80,100
{
  0,1,2,3,4,  0,  6,7,8,9,10
  
  
  
};

yqyPid_t tempPID;
void CopyPID(yqyPid_t* x,yqyPid_t* y)   //��x����������y
{
  y->kp = x->kp;
  y->ki = x->ki;
  y->kd = x->kd;
  y->iMax = x->iMax;
}

void AppLogData(int32_t data[4][11])
{
  uint8_t i = 0,p = 0;
  if(data == NULL)return;
  printf("AppLogData:\r\n");
  for(i = 0; i < 4; i++)
  {
    for(p = 0; p < 11; p++)
    {
      printf("%d,",data[i][p]);
    }
    printf("\n");
  }
  printf("AppLogData Finish\r\n");
}




#include "motor.h"
extern motor_t motor[4];                             //���ID  0,1,2,3
extern int32_t MotorValLab[4][11];
inline void MotorCtrl_Handle(uint8_t id)              //��id�������PID����������
{
  YQY_PID_Cal(&motor[id].pidSpd,motor[id].spd_mea,yqyAutoTest.testSpeed);
  YQY_PID_ZeroForce(&motor[id].pidSpd);
  motorCtrl_RAW(id,motor[id].pidSpd.out);
  HAL_Delay(PID_CALCULATE_ELAPS);
  yqyAutoTest.PIDlastError = motor[id].pidSpd.error;
}

void YQY_PID_AutoTest_OneMotor(uint8_t id)
{
  int32_t i = 0;
  static uint32_t StartTime;
  static uint32_t FullPowerTime;
  
  
  CopyPID(&motor[id].pidSpd,&tempPID);                         //����
  
  motor[id].pidSpd.kp = 2;                                   //����������PID����Ҫ���߻�������
  motor[id].pidSpd.ki = 0.25;
  motor[id].pidSpd.kd = 0;
  motor[id].pidSpd.iMax = 1000;
  
  yqyAutoTest.testSpeed = 0;
  if(USE_PRINTF)printf("Motor Auto Test Begin\r\n");
  
  do                                                                          //�ȴ����ֹͣ
  {
    MotorCtrl_Handle(id);
  }while((fabs(motor[id].pidSpd.error) > STEDY_ERROR_DEATH));
  HAL_Delay(100);
  if(USE_PRINTF)printf("Motor Stop Finish\r\n");
  
  for(i = 6; i < 11; i++)                                                       //��ת����
  {
    yqyAutoTest.testSpeed = (i - 5) * 20;     //20:20:100
    StartTime = HAL_GetTick();
    
    if(USE_PRINTF)printf("Motor SetSpeed:%.2f\r\n", yqyAutoTest.testSpeed);
    while(1)
    {
      
      if(fabs(motor[id].pidSpd.iMax - motor[id].pidSpd.iout) > 30)               //����û����ˢ��ȫ����ʱ��
        FullPowerTime = HAL_GetTick();
      if((fabs(motor[id].pidSpd.iMax - motor[id].pidSpd.iout) < 10) && (HAL_GetTick() - FullPowerTime > 3000))
      {                                                                         //�������ˣ�˵���������ˣ�������Ч
        if(USE_PRINTF)printf("Full Power!!\r\n");
        
        for(; i < 11; i++)                                                      //������Ԫ�ض�����Ϊ��ʱ��out
          MotorValTestLab[id][i] = motor[id].pidSpd.out;
        
        i = 12;
        
        break;
      }
      
      MotorCtrl_Handle(id);
      
      if(fabs(motor[id].pidSpd.error) > STEDY_ERROR_DEATH)StartTime = HAL_GetTick();   //���һֱ�ڣ���һֱˢ��
      if(HAL_GetTick() - StartTime > STEDY_ELAPS_TIME)                        //�ȶ�3�룬���¼�ò���
      {
        MotorValTestLab[id][i] = motor[id].pidSpd.out;
        break;
      }
    }
    if(USE_PRINTF)printf("Res:%.2f\r\n", motor[id].pidSpd.out);
  }
  
  yqyAutoTest.testSpeed = 0;
  do                                                                          //�ȴ����ֹͣ
  {
    MotorCtrl_Handle(id);
  }while((fabs(motor[id].pidSpd.error) > STEDY_ERROR_DEATH));
  
  
  for(i = 4; i >= 0; i--)                                                      //��ת����
  {
    yqyAutoTest.testSpeed = (i - 5) * 20;     //-20:20:-100
    StartTime = HAL_GetTick();
    
    if(USE_PRINTF)printf("Motor SetSpeed:%.2f\r\n", yqyAutoTest.testSpeed);
    while(1)
    {
      
      if(fabs(motor[id].pidSpd.iMax - motor[id].pidSpd.iout) > 30)               //����û����ˢ��ȫ����ʱ��
        FullPowerTime = HAL_GetTick();
      if((fabs(motor[id].pidSpd.iMax - motor[id].pidSpd.iout) < 10) && (HAL_GetTick() - FullPowerTime > 3000))
      {                                                                         //�������ˣ�˵���������ˣ�������Ч
        if(USE_PRINTF)printf("Full Power!!\r\n");
        
        for(; i >= 0; i--)                                                      //������Ԫ�ض�����Ϊ��ʱ��out
          MotorValTestLab[id][i] = motor[id].pidSpd.out;

        break;
      }
      
      MotorCtrl_Handle(id);
      
      if(fabs(motor[id].pidSpd.error) > STEDY_ERROR_DEATH)StartTime = HAL_GetTick();   //���һֱ�ڣ���һֱˢ��
      if(HAL_GetTick() - StartTime > STEDY_ELAPS_TIME)                          //�ȶ�3�룬���¼�ò���
      {
        MotorValTestLab[id][i] = motor[id].pidSpd.out;
        break;
      }
    }
    if(USE_PRINTF)printf("Res:%.2f\r\n", motor[id].pidSpd.out);
  }
  
  
  yqyAutoTest.testSpeed = 0;
  do                                                                          //�ȴ����ֹͣ
  {
    MotorCtrl_Handle(id);
  }while((fabs(motor[id].pidSpd.error) > STEDY_ERROR_DEATH));
  
  if(USE_PRINTF)printf("All Test Finish\r\n");                                //���У׼
  
  
  
  CopyPID(&tempPID,&motor[id].pidSpd);                                        //��ԭԭʼ��PID
  
                                                                              //��������
  SaveDataToLastPage((uint32_t*)MotorValTestLab, 44);
  ReadDataFromLastPage((uint32_t*)MotorValLab, 44);
  
  AppLogData(MotorValTestLab);                                                //��ӡ���
  AppLogData(MotorValLab);
  
  if(MotorValLab[id][3] == MotorValTestLab[id][3])                            //У�鱣����
  {
    if(USE_PRINTF)printf("Flash Saved! Check Pass\r\n");
  }
  else
  {
    if(USE_PRINTF)printf("Flash Saved ERROR! \r\n");
  }
  
}


