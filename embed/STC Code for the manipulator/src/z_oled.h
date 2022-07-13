/****************************************************************************
	*	@笔者	：	W
	*	@日期	：	2019年12月02日
	*	@所属	：	杭州众灵科技
	*	@论坛	：	www.ZL-robot.com
 ****************************************************************************/

#ifndef __OLED_H__
#define __OLED_H__

#include "z_stc15.h"
#include "z_delay.h"

/*******OLED管脚映射表*******/
sbit SCL=P2^5; //串行时钟
sbit SDA=P2^4; //串行数据

/*******OLED配置命令宏定义*******/
#define	Brightness	0xCF 
#define X_WIDTH 	128
#define Y_WIDTH 	64

/*******OLED相关函数声明*******/
void OLED_Write_Dat(unsigned char I2C_Data);
void OLED_Write_Cmd(unsigned char I2C_Command);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_Data);
void OLED_CLS(void);
void OLED_Init(void);
void OLED_P6x8Str(unsigned char x,unsigned char y,unsigned char ch[]);
void OLED_P8x16Str(unsigned char x,unsigned char y,unsigned char ch[]);
void OLED_P16x16Ch(unsigned char x,unsigned char y,unsigned char N);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
void OLED_TEST(void);

#endif

