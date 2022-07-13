#ifndef __IO_H__
#define __IO_H__

#include "z_stc15.h"
#include "z_delay.h"

sbit nled = P5^3;
sbit beep = P2^6;

sbit dj0 = P0^5;
sbit dj1 = P0^6;
sbit dj2 = P0^7;
sbit dj3 = P3^5;
sbit dj4 = P3^6;
sbit dj5 = P3^7;

sbit rst_io = P5^4;

sbit key1 = P3^2;
sbit key2 = P3^3;

#define nled_on() {nled = 0;}
#define nled_off() {nled = 1;}
#define nled_switch() {nled = ~nled;}

#define beep_on() 		{beep = 1;}
#define beep_off() 		{beep = 0;}
#define beep_switch() 	{beep = ~beep;}

void io_init(void);
void dj_io_init(void);
void dj_io_set(u8 index, u8 level);
void beep_on_times(int times, int delay);


#endif