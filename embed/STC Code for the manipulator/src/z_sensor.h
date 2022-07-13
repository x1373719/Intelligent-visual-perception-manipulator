#ifndef __SENSOR_H__
#define __SENSOR_H__

#include "z_stc15.h"


#define AI_Read() AI_pin


sbit AI_pin  = P4^4; 		//产生脉冲引脚
void AI_jiaqu(void);


#endif