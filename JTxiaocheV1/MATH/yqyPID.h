#ifndef __YQY_PID_H
#define __YQY_PID_H



typedef struct
{
	float kp,ki,kd;                     //kp,ki,kd  （需要初始化）
	float error,target,lastMeasure;     //测量值
	float pout,iout,dout,out;           //输出

  float iSep,iMax;                    //积分分离值  (需要初始化)
  float outMax;                       //输出限幅    (需要初始化)
  float lastTarget;                   //用于前馈控制，不许自行赋值
}yqyPid_t;

void YQY_ResetPID(yqyPid_t* YQYpid,float kp,float ki,float kd);       //重置kp ki kd
float YQY_PID_Cal(yqyPid_t* YQYpid,float measure, float target);      //PID计算
void YQY_PID_ZeroForce(yqyPid_t* YQYpid);                             //快速归零


#endif
