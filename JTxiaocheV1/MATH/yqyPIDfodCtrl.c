#include "yqyPIDfodCtrl.h"
#include "yqyPID.h"
#include "stdio.h"
#include "math.h"

int32_t MotorValLab[4][11] =                          //20,40,60,80
{
  -970,-901,-817,-599,-491,0,252,359,497,644,815,     //电压5V下的校准表
  -686,-552,-422,-292,-159,0,305,386,468,563,699,
  -970,-906,-826,-732,-588,0,242,389,537,686,830,
  -731,-593,-462,-331,-207,0,299,378,469,595,752,
};

//-504,-449,-348,-285,-264,0,104,153,207,261,315,     //电压12V下的校准表
//-270,-217,-168,-119,-66,0,161,198,222,240,265,
//-544,-487,-429,-345,-305,0,97,153,211,270,325,
//-279,-231,-183,-134,-82,0,153,185,214,239,261,




static inline float remap(float value, float x1, float x2, float y1, float y2) 
 {
   float mappedValue = 0;
    if (value < x1)value = x1;
    if (value > x2)value = x2;
    mappedValue = y1 + ((value - x1) / (x2 - x1)) * (y2 - y1);

    return mappedValue;
}
 
void AutoTestValLab(void)
{
  
  
}

#define RESTRAIN_X(x, step) do { \
    if ((x) > 0) { \
        (x) -= (step); \
        if ((x) < 0) (x) = 0; \
    } \
    if ((x) < 0) { \
        (x) += (step); \
        if ((x) > 0) (x) = 0; \
    } \
} while (0)

#include "motor.h"
float AddVal = 0;
inline void YQY_PID_FeedForwardCtrl(yqyPid_t* YQYpid)
{
  uint8_t i = 0;
  uint8_t id = 0;
  
  if(YQYpid == &motor[0].pidSpd)id = 0;
  if(YQYpid == &motor[1].pidSpd)id = 1;
  if(YQYpid == &motor[2].pidSpd)id = 2;
  if(YQYpid == &motor[3].pidSpd)id = 3;
  
  if(fabs(YQYpid->target) > 100)return;
  if(fabs(YQYpid->lastTarget - YQYpid->target) < 2)return;
  
  YQYpid->lastTarget = YQYpid->target;
  
  i = (YQYpid->target + 100) / 20;
  
  AddVal = remap(YQYpid->target, (i-5) * 20, (i-4) * 20, MotorValLab[id][i],MotorValLab[id][i + 1]);
  YQYpid->iout = AddVal * 0.8;
}



