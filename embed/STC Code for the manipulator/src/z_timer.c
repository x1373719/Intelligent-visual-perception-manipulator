#include <stdio.h>
#include "z_timer.h"
#include "z_global.h"
#include "z_gpio.h"

#define FOSC 22118400L  
static u32 systick_ms = 0;

void timer0_init(void) {
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TMOD |= 0x01;		//设置定时器模式
	TL0 = 0XAA;			//设置定时初值
	TH0 = 0XAA;			//设置定时初值
	TF0 = 0;			//清除TF0标志
	TR0 = 1;			//定时器0开始计时
	ET0 = 1; 			//打开中断
	EA =  1;
}



void timer1_init(void) {
	AUXR |= 0x40;		//定时器时钟1T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0;		//设置定时初值
	TH1 = 0;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1 = 1; 		//打开中断
	EA  = 1;
}

void timer3_init(void)		//1毫秒@22.1184MHz
{
	T4T3M |= 0x02;		//定时器时钟1T模式
	T3L = 0x9A;			//设置定时初值
	T3H = 0xA9;			//设置定时初值
	T4T3M |= 0x08;		//定时器3开始计时
	IE2 |= 0X20;		//打开外部中断
	EA	= 1;			//总开关
}


void timer0_reset(int t_us) {
	//本来应该x22.1184 但由于单片机用的内部晶振，有一定误差，调整到下面这个值 频率差不多50HZ
	TL0 = (int)(65535-20.7*t_us);
	TH0 = (int)(65535-20.7*t_us) >> 8;
}

u32 millis(void) {
	return systick_ms;
}

float abs_float(float value) {
	if(value>0) {
		return value;
	}
	return (-value);
}

void duoji_inc_handle(u8 index) {	
	int aim_temp;
	
	if(duoji_doing[index].inc != 0) {
		
		aim_temp = duoji_doing[index].aim;
		
		if(aim_temp > 2490){
			aim_temp = 2490;
		} else if(aim_temp < 500) {
			aim_temp = 500;
		}
	
		if(abs_float(aim_temp - duoji_doing[index].cur) <= abs_float(duoji_doing[index].inc + duoji_doing[index].inc)) {
			duoji_doing[index].cur = aim_temp;
			duoji_doing[index].inc = 0;
		} else {
			duoji_doing[index].cur += duoji_doing[index].inc;
		}
	}
}

void T0_IRQ(void) interrupt 1 {
	static volatile u8 flag = 0, count=0;
	int temp;

	if(duoji_index1 >= 8) {
		duoji_index1 = 0;
	}
		
	if(!flag) {
		//有偏差的时候就发送PWM脉冲
		if(duoji_doing[duoji_index1].inc || millis() < 3000){
			timer0_reset((unsigned int)(duoji_doing[duoji_index1].cur));
			dj_io_set(duoji_index1, 1);
			duoji_inc_handle(duoji_index1);
			count = 0;
		} else {
			count++;
			if(count < 3) {
				timer0_reset((unsigned int)(duoji_doing[duoji_index1].cur));
				dj_io_set(duoji_index1, 1);
				duoji_inc_handle(duoji_index1);
			} else {
				count = 3;
			}
		}
		
	} else {
		temp = 2500 - (unsigned int)(duoji_doing[duoji_index1].cur);
		if(temp < 20)temp = 20;
		timer0_reset(temp);
		dj_io_set(duoji_index1, 0);
		duoji_index1 ++;
	}
	flag = !flag;
}

//超声波定时中断处理
void T1_IRQ(void) interrupt 3{
	//timer0_reset(1000);
	//systick_ms ++;
	TL1 = 0;		//设置定时初值
	TH1 = 0;		//设置定时初值
	csb_cnt++;
}

void T3_IRQ(void) interrupt 19{
	systick_ms ++;
}


