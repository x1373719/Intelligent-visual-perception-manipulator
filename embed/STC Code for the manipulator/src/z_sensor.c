#include <stdio.h>
#include <string.h>

#include "z_sensor.h"
#include "z_adc.h"
#include "z_global.h"
#include "z_gpio.h"
#include "z_pwm.h"
#include "z_timer.h"
#include "z_uart.h"
#include "z_main.h"
#include "z_w25q64.h"



/*
	智能功能代码
*/

//P44

//初始化传感器IO口
void setup_sensor(void) {
//	P4M1 &= ~(1<<0);
//	P4M0 &= ~(1<<0);
	//不需要配置，默认准双向I/O口
}

void AI_jiaqu(void) {
	if(group_do_ok == 0)return;//有动作执行，直接返回
//	sprintf((char *)uart_receive_buf, "AI_Read() = %d\r\n", AI_Read());
//	uart1_send_str(uart_receive_buf);
	//mdelay(500);
	if(AI_Read() == 0) {
		mdelay(20);
	  if(AI_Read() == 0) {
			parse_cmd((u8 *)"$DGT:1-9,1!");
			beep_on_times(1, 100);
		} 
	}
}
