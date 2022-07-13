#ifndef __PS2_H__
#define __PS2_H__

#include "z_stc15.h"

#define START_CMD			0x01
#define ASK_DAT_CMD			0x42

//IO口定义
sbit  PS2_DAT=P0^4;	
sbit  PS2_CMD=P0^3;
sbit  PS2_ATT=P0^2;
sbit  PS2_CLK=P0^1;

 /*********************************************************

	函数声明

**********************************************************/	 

void psx_init(void);
void psx_write_read(unsigned char *get_buf);




#endif