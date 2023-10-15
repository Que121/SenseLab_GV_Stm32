#include "yqyPID.h"
#include "math.h"
#define LimitMax(input, max)   \
    {                          \
        if (input > max)       \
        {                      \
            input = max;       \
        }                      \
        else if (input < -max) \
        {                      \
            input = -max;      \
        }                      \
    }
void YQY_ResetPID(yqyPid_t* YQYpid,float kp,float ki,float kd)
{
	YQYpid -> kp = kp;
	YQYpid -> ki = ki;
	YQYpid -> kd = kd;
}
float YQY_PID_Cal(yqyPid_t* YQYpid,float measure, float target)
{
  YQYpid->target = target;
	YQYpid->error = YQYpid->target - measure;
	
	YQYpid->pout = YQYpid->error * YQYpid->kp;
	if((YQYpid->error > YQYpid->iSep) || (YQYpid->error < -YQYpid->iSep))
		YQYpid->iout += YQYpid->error * YQYpid->ki;
	YQYpid->dout = (measure - YQYpid->lastMeasure) * YQYpid->kd;
	YQYpid->out = YQYpid->pout + YQYpid->iout + YQYpid->dout;
	
	LimitMax(YQYpid->iout, YQYpid->iMax);		    //积分限幅
	LimitMax(YQYpid->out, YQYpid->outMax);		  //输出限幅
	
	YQYpid->lastMeasure = measure;			        //更新上次测量值
  return YQYpid->out;
}
inline void YQY_PID_ZeroForce(yqyPid_t* YQYpid)
{
  if(fabs(YQYpid->target) <= 1)
  {
    if(fabs(YQYpid->error) > 40)
      YQYpid->iout += YQYpid->error * 3;
    if(fabs(YQYpid->error) < 20)
    {
      YQYpid->iout = 0;
      YQYpid->out = 0;
    }
  }
}



