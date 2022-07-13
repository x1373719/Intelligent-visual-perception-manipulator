

#include <stdio.h>
#include <string.h>
#include <intrins.h>
#include "z_stc15.h"
#include "z_main.h"
#include "z_uart.h"
#include "z_delay.h"
#include "z_gpio.h"
#include "z_ps2.h"
#include "z_timer.h"
#include "z_w25q64.h"
#include "z_global.h"
#include "z_adc.h"
#include "z_sensor.h"
#include "z_kinematics.h"	//逆运动学算法


u8 i;
u16 do_start_index, do_time, group_num_start, group_num_end, group_num_times;
u32 bias_systick_ms_bak = 0;
u8 needSaveFlag = 0;
u32 save_addr_sector = 0, save_action_index_bak = 0;
u8 psx_buf[9]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}; 	
u32 downLoadTime = 0;
u32 action_time = 0;

code const char *pre_cmd_set_red[PSX_BUTTON_NUM] = {
	"<PS2_RED01:#005P0600T2000!^#005PDST!>",	//L2						  
	"<PS2_RED02:#005P2400T2000!^#005PDST!>",	//R2						  
	"<PS2_RED03:#004P0600T2000!^#004PDST!>",	//L1						  
	"<PS2_RED04:#004P2400T2000!^#004PDST!>",	//R1			
	"<PS2_RED05:#002P2400T2000!^#002PDST!>",	//RU						  
	"<PS2_RED06:#003P2400T2000!^#003PDST!>",	//RR						  
	"<PS2_RED07:#002P0600T2000!^#002PDST!>",	//RD						  
	"<PS2_RED08:#003P0600T2000!^#003PDST!>",	//RL				
	"<PS2_RED09:$DJR!>",					    //SE    				  
	"<PS2_RED10:>",					            //AL						   
	"<PS2_RED11:>",					            //AR						  
	"<PS2_RED12:$DJR!>",					    //ST		
	"<PS2_RED13:#001P0600T2000!^#001PDST!>",	//LU						  
	"<PS2_RED14:#000P0600T2000!^#000PDST!>",	//LR								  
	"<PS2_RED15:#001P2400T2000!^#001PDST!>",	//LD						  
	"<PS2_RED16:#000P2400T2000!^#000PDST!>",	//LL	
};

code const char *pre_cmd_set_grn[PSX_BUTTON_NUM] = {
	"<PS2_GRN01:#005P0600T2000!^#005PDST!>",	//L2						  
	"<PS2_GRN02:#005P2400T2000!^#005PDST!>",	//R2						  
	"<PS2_GRN03:#004P0600T2000!^#004PDST!>",	//L1						  
	"<PS2_GRN04:#004P2400T2000!^#004PDST!>",	//R1			
	"<PS2_GRN05:#002P2400T2000!^#002PDST!>",	//RU						  
	"<PS2_GRN06:#003P2400T2000!^#003PDST!>",	//RR						  
	"<PS2_GRN07:#002P0600T2000!^#002PDST!>",	//RD						  
	"<PS2_GRN08:#003P0600T2000!^#003PDST!>",	//RL				
	"<PS2_GRN09:$DJR!>",					    //SE    				  
	"<PS2_GRN10:>",					            //AL						   
	"<PS2_GRN11:>",					            //AR						  
	"<PS2_GRN12:$DJR!>",					    //ST		
	"<PS2_GRN13:#001P0600T2000!^#001PDST!>",	//LU						  
	"<PS2_GRN14:#000P0600T2000!^#000PDST!>",	//LR								  
	"<PS2_GRN15:#001P2400T2000!^#001PDST!>",	//LD						  
	"<PS2_GRN16:#000P2400T2000!^#000PDST!>",	//LL							  
};

/*
	代码从main里开始执行
	在进入大循环while(1)之前都为各个模块的初始化
	最后在大循环处理持续执行的事情
	另外注意uart中的串口中断，接收数据处理
	timer中的定时器中断，舵机的脉冲收发就在那里
*/

kinematics_t kinematics;
int ret;

void main(void) {
	setup_global();			//初始化全局变量
	setup_gpio();			//初始化IO口
	setup_djio();			//初始化舵机IO口
	setup_nled();			//初始化工作指示灯
	setup_beep();			//初始化定时器	
	
	setup_w25q64();			//初始化存储器W25Q64
	setup_ps2();			//初始化PS2手柄
	setup_uart1();			//初始化串口1
	setup_uart2();			//初始化串口2
	
	setup_systick();		//初始化滴答时钟，1S增加一次systick_ms的值
	
	setup_servo_bias();		//初始化偏差
	setup_dj_timer();		//初始化定时器0 处理舵机PWM输出
	setup_interrupt();		//初始化总中断

	setup_sensor();			//初始化传感器IO口
	
	setup_start();			//初始化启动信号
	
	//kinematics 90mm 105mm 98mm 150mm
	setup_kinematics(90, 105, 98, 150, &kinematics);
	
    while(1) {
		loop_nled();							//循环执行工作指示灯，500ms跳动一次.
		loop_uart();							//串口数据接收处理
		if(millis() - downLoadTime > 1000) {	//保证下载的时候不受其他功能的干扰
			loop_action();						//动作组批量执行
			loop_bt_once();						//蓝牙修改波特率和名称
			loop_ps2_data();					//循环读取PS2手柄数据
			loop_ps2_button();				//处理手柄上的按钮
			loop_monitor();						//定时保存一些变量
//			AI_jiaqu();						    //传感器处理
		}	
	}
}

//--------------------------------------------------------------------------------
/*
	初始化函数实现
*/
//初始化全局变量
void setup_global(void) {
	//全局变量初始化
	global_init();
}
//初始化IO口
void setup_gpio(void) {
	//IO初始化
	io_init();
}
//初始化工作指示灯 初始化已在io_init中初始化
void setup_nled(void) {
	nled_off();			//工作指示灯关闭
}
//初始化蜂鸣器 初始化已在io_init中初始化
void setup_beep(void) {
	beep_off();			//关闭蜂鸣器
}			
//初始化舵机IO口
void setup_djio(void) {
	dj_io_init();		//舵机IO口初始化
}	

//初始化存储器W25Q64
void setup_w25q64(void) {
	//存储器初始化，读取ID进行校验，若错误则长鸣不往下执行
	w25x_init();
	while(w25x_readId()!= W25Q64)beep_on();
	
	w25x_read((u8 *)(&eeprom_info), W25Q64_INFO_ADDR_SAVE_STR, sizeof(eeprom_info_t));	//读取全局变量
	if(eeprom_info.version != VERSION) {	//判断版本是否是当前版本
		eeprom_info.version = VERSION;		//复制当前版本
		eeprom_info.dj_record_num = 0;		//学习动作组变量赋值0
	} 
	
	if(eeprom_info.dj_bias_pwm[DJ_NUM] != FLAG_VERIFY) {
		for(i=0;i<DJ_NUM;i++) {
			eeprom_info.dj_bias_pwm[i] = 0;
		}
		eeprom_info.dj_bias_pwm[DJ_NUM] = FLAG_VERIFY;
	}
	
	if(eeprom_info.digtal_mode > 1) {
		eeprom_info.digtal_mode = 0;
	}
}	

//初始化PS2手柄
void setup_ps2(void) {
	//手柄初始化
	psx_init();
}
//初始化定时器2 处理舵机PWM输出
void setup_dj_timer(void) {
	timer0_init();	//舵机 定时器初始化
}

//初始化串口1
void setup_uart1(void) {
	//串口1初始化
	uart1_init(115200);
	//uart1_close();
	uart1_open();
	//串口发送测试字符
	uart1_send_str((u8 *)"uart1 check ok!");
}
//初始化串口2
void setup_uart2(void) {
	//串口2初始化
	uart2_init(115200);
	//uart2_close();
	uart2_open();
	//串口发送测试字符
	uart2_send_str((u8 *)"uart2 check ok!");
}	
//初始化串口4
void setup_uart4(void) {
	//串口4初始化
	uart4_init(115200);
	//uart4_close();
	uart4_open();
	
	//串口发送测试字符
	uart4_send_str((u8 *)"uart4 check ok!");
}	
//初始化滴答时钟，1S增加一次systick_ms的值
void setup_systick(void) {
	//系统滴答时钟初始化	
	timer3_init();
}


//初始化启动信号
void setup_start(void) {
	
	//蜂鸣器LED 名叫闪烁 示意系统启动
	beep_on();nled_on();mdelay(200);beep_off();nled_off();mdelay(200);
	beep_on();nled_on();mdelay(200);beep_off();nled_off();mdelay(200);
	beep_on();nled_on();mdelay(200);beep_off();nled_off();mdelay(200);
	
	//执行预存命令 {G0000#000P1500T1000!#000P1500T1000!}
	if(eeprom_info.pre_cmd[PRE_CMD_SIZE] == FLAG_VERIFY) {
		strcpy((char *)uart_receive_buf, (char *)eeprom_info.pre_cmd);
		if(eeprom_info.pre_cmd[0] == '$') {
			parse_cmd(eeprom_info.pre_cmd);
		} else {
			for(i=16;i<strlen((char *)uart_receive_buf);i+=15) {
				uart_receive_buf[i] = '0';
				uart_receive_buf[i+1] = '0';
				uart_receive_buf[i+2] = '0';
				uart_receive_buf[i+3] = '0';
			}
			parse_action(uart_receive_buf);
		}
	}
	
	
}	
//初始化其他
void setup_servo_bias(void) {	
	
	//将偏差带入初始值
	for(i=0;i<DJ_NUM;i++) {
		duoji_doing[i].aim = 1500+eeprom_info.dj_bias_pwm[i];
		duoji_doing[i].cur = duoji_doing[i].aim;
		duoji_doing[i].inc = 0;		
	}
}

//初始化总中断
void setup_interrupt(void) {
	//串口1设为高优先级
	IP = 0X10;
	//IP2 = 0X01;
	//总中断打开
	EA = 1;
}	
//--------------------------------------------------------------------------------


//--------------------------------------------------------------------------------
/*
	主循环函数实现
*/
//循环执行工作指示灯，500ms跳动一次
void loop_nled(void) {
	static u32 systick_ms_bak = 0;
	if(millis() - systick_ms_bak >= 500) {
		systick_ms_bak = millis();
		nled_switch();	
	}
}		
//串口数据接收处理
void loop_uart(void) {
	if(uart1_get_ok) {
		//测试发回去
		//uart1_send_str(uart_receive_buf);

		if(uart1_mode == 1) {				//命令模式
			//uart1_send_str(">cmd");
			parse_cmd(uart_receive_buf);			
		} else if(uart1_mode == 2 || uart1_mode == 3) {		//单个舵机模式 多个舵机模式
			//uart1_send_str(">sig");
			parse_action(uart_receive_buf);
		} else if(uart1_mode == 4) {		//保存模式
			//uart1_send_str(">save");
			//uart1_send_str(uart_receive_buf);
			action_save(uart_receive_buf);
			downLoadTime = millis();
		} 
		
		memset(uart_receive_buf, 0, sizeof(uart_receive_buf));
		uart1_mode = 0;
		uart1_get_ok = 0;
	}
	
	return;
}	

//定时保存一些变量
void loop_monitor(void) {
	static u32 saveTime = 3000;
	if((needSaveFlag == 1) || (millis() - bias_systick_ms_bak > saveTime)) {
		needSaveFlag = 0;
		bias_systick_ms_bak = millis();
		rewrite_eeprom();
	}	
	return;
}	


void loop_ps2_data(void) {
	static u32 systick_ms_bak = 0;
	if(millis() - systick_ms_bak < 50) {
		return;
	}
	systick_ms_bak = millis();
	psx_write_read(psx_buf);
#if 0	
	sprintf(cmd_return, "0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x\r\n", 
	(int)psx_buf[0], (int)psx_buf[1], (int)psx_buf[2], (int)psx_buf[3],
	(int)psx_buf[4], (int)psx_buf[5], (int)psx_buf[6], (int)psx_buf[7], (int)psx_buf[8]);
	uart1_send_str(cmd_return);
#endif 	
	
	return;
}

int __abs_int(int a) {
	if(a>0)return (a);
	return (-a);
}

void loop_ps2_joystick(void) {
	static int joystick_left_LR_bak=0, joystick_right_UD_bak=0, joystick_left_UD_bak;
	static int joystick_left_LR, joystick_right_UD, joystick_left_UD;
	
	if(psx_buf[1] != PS2_LED_RED)return;
	
	joystick_left_UD = (psx_buf[8]-127) * 8;
	joystick_left_LR = (psx_buf[7]-127) * 8;
	joystick_right_UD = (psx_buf[6]-127) * 8;

	if(__abs_int(joystick_left_LR - joystick_left_LR_bak) > 16 || __abs_int(joystick_left_UD - joystick_left_UD_bak) > 16) {
//		sprintf((char *)uart_receive_buf, "{#000P%04dT0100!#001P%04dT0000!#002P%04dT1000!#003P%04dT0000!#004P%04dT0000!#005P%04dT0000!}", 
//		1500+(psx_buf[7]-127) * 6,
//		1500+(psx_buf[8]-127) * 4,1500-(psx_buf[8]-127) * 4,1500-(psx_buf[8]-127) * 4,1500+(psx_buf[8]-127) * 4,1500+(psx_buf[8]-127) * 4);
		if(__abs_int(joystick_left_LR)<30)joystick_left_LR=0;
		sprintf((char *)uart_receive_buf, "#000P%04dT1000!#001P%04dT1000!#002P%04dT1000!#003P%04dT1000!#004P%04dT1000!#005P%04dT1000!", 
		(int)(1500-joystick_left_LR*0.6),
		(int)(1500-joystick_left_UD*0.6),
		(int)(1500+joystick_left_UD*0.6),
		(int)(1500+joystick_left_UD*0.6),
		(int)(1500-joystick_left_UD*0.6),
		(int)(1500-joystick_left_UD*0.6));
		parse_action(uart_receive_buf);
		//uart1_send_str(uart_receive_buf);
		//zx_uart_send_str(cmd_return);
		
		joystick_left_LR_bak = joystick_left_LR;
		joystick_left_UD_bak = joystick_left_UD;
	}
	
	if(__abs_int(joystick_right_UD-joystick_right_UD_bak) > 16) {
		joystick_left_LR = (psx_buf[6]-127) * 4 - (psx_buf[5]-127) * 4;
		joystick_right_UD = (psx_buf[6]-127) * 4 + (psx_buf[5]-127) * 4;
		
		//sprintf((char *)uart_receive_buf, "{#000P%04dT0000!}", 1500+(joystick_left_LR-joystick_right_UD));
		//parse_action(uart_receive_buf);
		
		car_set(joystick_left_LR, joystick_right_UD);
		joystick_right_UD_bak = joystick_right_UD;
	}

}



void loop_ps2_button(void) {
	static unsigned char psx_button_bak[2] = {0};

	if((psx_button_bak[0] == psx_buf[3])
	&& (psx_button_bak[1] == psx_buf[4])) {			
	} else {
		parse_psx_buf(psx_buf+3, psx_buf[1]);
		//uart1_send_str("changed\r\n");
		psx_button_bak[0] = psx_buf[3];
		psx_button_bak[1] = psx_buf[4];
	}
	return;
}

//处理小车电机摇杆控制
void loop_ps2_car(void) {
	static int car_left, car_right, car_left_bak, car_right_bak;
	
	if(psx_buf[1] != PS2_LED_RED)return;
	
	if(abs_int(127 - psx_buf[8]) < 5 )psx_buf[8] = 127;
	if(abs_int(127 - psx_buf[6]) < 5 )psx_buf[6] = 127;
	
	car_left = (127 - psx_buf[8]) * 8;
	car_right = (127 - psx_buf[6]) * 8;
  if (car_left > 1000) {
    car_left = 1000;
  } else if (car_left < -1000) {
    car_left = -1000;
  } else if (car_right > 1000) {
    car_right = 1000;
  } else if (car_right < -1000) {
    car_right = -1000;
  }	
	if(car_left != car_left_bak || car_right != car_right_bak) {
		//uart1_send_str((u8*)"ps2:");
		car_set(car_left, car_right);
		car_left_bak = car_left;
		car_right_bak = car_right;
	}
}

void parse_psx_buf(unsigned char *buf, unsigned char mode) {
	u8 i, pos = 0;
	static u16 bak=0xffff, temp, temp2;
	temp = (buf[0]<<8) + buf[1];
	
	if(bak != temp) {
		temp2 = temp;
		temp &= bak;
		for(i=0;i<16;i++) {
			if((1<<i) & temp) {
			} else {
				if((1<<i) & bak) {	//press
															
					memset(uart_receive_buf, 0, sizeof(uart_receive_buf));					
					if(mode == PS2_LED_RED) {
						memcpy((char *)uart_receive_buf, (char *)pre_cmd_set_red[i], strlen(pre_cmd_set_red[i]));
					} else if(mode == PS2_LED_GRN) {
						memcpy((char *)uart_receive_buf, (char *)pre_cmd_set_grn[i], strlen(pre_cmd_set_grn[i]));
					} else continue;
					
					pos = str_contain_str(uart_receive_buf, "^");
					if(pos) uart_receive_buf[pos-1] = '\0';
					if(str_contain_str(uart_receive_buf, "$")) {
						//uart1_close();
						//uart1_get_ok = 1;
						//uart1_mode = 1;
						strcpy(cmd_return, uart_receive_buf+11);
						strcpy(uart_receive_buf, cmd_return);
						parse_cmd(uart_receive_buf);
					} else if(str_contain_str(uart_receive_buf, "#")) {
						//uart1_close();
						//uart1_get_ok = 1;
						//uart1_mode = 2;
						strcpy(cmd_return, uart_receive_buf+11);
						strcpy(uart_receive_buf, cmd_return);
						parse_action(uart_receive_buf);
					}
					
					//uart1_send_str(uart_receive_buf);
					//zx_uart_send_str(uart_receive_buf);
					
					bak = 0xffff;
				} else {//release
										
					memset(uart_receive_buf, 0, sizeof(uart_receive_buf));					
					if(mode == PS2_LED_RED) {
						memcpy((char *)uart_receive_buf, (char *)pre_cmd_set_red[i], strlen(pre_cmd_set_red[i]));
					} else if(mode == PS2_LED_GRN) {
						memcpy((char *)uart_receive_buf, (char *)pre_cmd_set_grn[i], strlen(pre_cmd_set_grn[i]));
					} else continue;	
					
					pos = str_contain_str(uart_receive_buf, "^");
					if(pos) {
						if(str_contain_str(uart_receive_buf+pos, "$")) {
							//uart1_close();
							//uart1_get_ok = 1;
							//uart1_mode = 1;
							strcpy(cmd_return, uart_receive_buf+pos);
							cmd_return[strlen(cmd_return) - 1] = '\0';
							strcpy(uart_receive_buf, cmd_return);
							parse_cmd(uart_receive_buf);
						} else if(str_contain_str(uart_receive_buf+pos, "#")) {
							//uart1_close();
							//uart1_get_ok = 1;
							//uart1_mode = 2;
							strcpy(cmd_return, uart_receive_buf+pos);
							cmd_return[strlen(cmd_return) - 1] = '\0';
							strcpy(uart_receive_buf, cmd_return);
							parse_action(uart_receive_buf);
						}
						//uart1_send_str(uart_receive_buf);
						//zx_uart_send_str(uart_receive_buf);
					}	
				}
				//测试执行指令
				//uart1_send_str(uart_receive_buf);

			}
		}
		bak = temp2;
		beep_on();mdelay(50);beep_off();
	}	
	return;
}

void handle_uart(void) {


	return;
}

/*
	$DST!
	$DST:x!
	$RST!
	$CGP:%d-%d!
	$DEG:%d-%d!
	$DGS:x!
	$DGT:%d-%d,%d!
	$DCR:%d,%d!
	$DWA!
	$DWD!
	$DJR!
	$GETA!
*/

void parse_cmd(u8 *cmd) {
	static u8 djrFlag=0;
	u16 pos, i, index;
	int int1, int2, int3, int4;
	
	uart1_send_str(cmd);
	
	if(pos = str_contain_str(cmd, "$DST!"), pos) {
		group_do_ok  = 1;
		for(i=0;i<DJ_NUM;i++) {
			duoji_doing[i].inc = 0;	
			duoji_doing[i].aim = duoji_doing[i].cur;
		}
		zx_uart_send_str("#255PDST!");
		car_set(0, 0);
		AI_mode = 255;
	} else if(pos = str_contain_str(cmd, "$DST:"), pos) {
		if(sscanf(cmd, "$DST:%d!", &index)) {
			duoji_doing[index].inc = 0;	
			duoji_doing[index].aim = duoji_doing[index].cur;
			sprintf(cmd_return, "#%03dPDST!", (int)index);
			zx_uart_send_str(cmd_return);
		}
		
		
	} else if(pos = str_contain_str(cmd, "$RST!"), pos) {		
		soft_reset();
	} else if(pos = str_contain_str(cmd, "$PTG:"), pos) {		
		if(sscanf(cmd, "$PTG:%d-%d!", &int1, &int2)) {
			print_group(int1, int2);
		}
	} else if(pos = str_contain_str(cmd, "$DEG:"), pos) {		
		if(sscanf(cmd, "$DEG:%d-%d!", &int1, &int2)) {
			erase_sector(int1, int2);
		}
	} else if(pos = str_contain_str(cmd, "$DGS:"), pos) {		
		if(sscanf(cmd, "$DGS:%d!", &int1)) {
			do_group_once(int1);
			group_do_ok = 1;
		}
	} else if(pos = str_contain_str(cmd, "$DGT:"), pos) {		
		if(sscanf((char *)cmd, "$DGT:%d-%d,%d!", &group_num_start, &group_num_end, &group_num_times)) {
			//uart1_send_str("111111");			
			if(group_num_start != group_num_end) {
				do_start_index = group_num_start;
				do_time = group_num_times;
				group_do_ok = 0;
				//uart1_send_str("22222");
			} else {
				group_do_ok = 1;
				do_group_once(group_num_start);
				//uart1_send_str("33333");
			}
		}
	} else if(pos = str_contain_str(cmd, "$DCR:"), pos) {		
		if(sscanf(cmd, "$DCR:%d,%d!", &int1, &int2)) {
			car_set(int1, int2);
		}
	} else if(pos = str_contain_str(cmd, "$DJR!"), pos) {	
		zx_uart_send_str("#255P1500T2000!");		
		//将偏差带入初始值
		for(i=0;i<DJ_NUM;i++) {
			duoji_doing[i].aim = 1500+eeprom_info.dj_bias_pwm[i];
			duoji_doing[i].inc = (duoji_doing[i].aim -  duoji_doing[i].cur) / (duoji_doing[i].time/20.000);
		}
	} else if(pos = str_contain_str(cmd, "$JXB_SWITCH!"), pos) {	
		group_do_ok = 1;
		zx_uart_send_str("#255P1500T2000!");		
		for(i=0;i<DJ_NUM;i++) {
			duoji_doing[i].aim  = 1500;
			duoji_doing[i].time = 2000;
			duoji_doing[i].inc = (duoji_doing[i].aim -  duoji_doing[i].cur) / (duoji_doing[i].time/20.000);
		}
		
		if(djrFlag) {
			do_group_once(1);
		} else {
			do_group_once(2);
		}
		djrFlag = !djrFlag;
	} else if(pos = str_contain_str(cmd, "$GETA!"), pos) {		
		uart1_send_str("AAA");
	} else if(pos = str_contain_str(cmd, "$GETS!"), pos) {		
		if(group_do_ok == 0) {
			uart1_send_str("group_do_ok=0");
		} else {
			uart1_send_str("group_do_ok=1");
		}
	} else if(pos = str_contain_str(cmd, "$GETINC!"), pos) {		
		for(i=0;i<8;i++) {
			sprintf(cmd_return, "inc%d = %f \r\n", (int)i, duoji_doing[i].inc);
			uart1_send_str(cmd_return);
		}
	} else if(pos = str_contain_str(cmd, "$DRS!"), pos) {	
		uart1_send_str("\r\n51MCU-IAP15W4K61S4 Ver:");
		uart1_send_str(VERSION);
	} else if(pos = str_contain_str(cmd, (u8 *)"$SMODE"), pos) {		
		if(sscanf((char *)cmd, "$SMODE%d!", &int1)) {
			if(int1 < 10) {
				AI_mode = int1;
				beep_on_times(1, 100);
				car_set(0,0);
			}
		}
	} else if(pos = str_contain_str(cmd, (u8 *)"$SMART_STOP!"), pos) {		
		AI_mode = 255;
		beep_on_times(1, 100);
		parse_action((u8 *)"#255PDST!");
		uart1_send_str((u8 *)"@OK!");
	}  else if(pos = str_contain_str(cmd, (u8 *)"$KMS:"), pos) {		
		if(sscanf((char *)cmd, "$KMS:%d,%d,%d,%d!", &int1, &int2, &int3, &int4)) {
			uart1_send_str((u8 *)"Try to find best pos:\r\n");
			if(kinematics_move(0, int2, int3, int4)) {
				beep_on();mdelay(50);beep_off();mdelay(50);
			} else {
				beep_on();mdelay(50);beep_off();mdelay(50);
				beep_on();mdelay(50);beep_off();mdelay(50);
				uart1_send_str((u8 *)"Can't find best pos!!!");
			}					
		}
	}
}



void action_save(u8 *str) {
	int action_index = 0;
	//预存命令处理
	
	if(str[1] == '$' && str[2] == '!') {
		eeprom_info.pre_cmd[PRE_CMD_SIZE] = 0;
		rewrite_eeprom();
		uart1_send_str((u8 *)"@CLEAR PRE_CMD OK!");
		return;
	} else if(str[1] == '$') {
		if(sscanf((char *)str, "<$DGT:%d-%d,%d!>", &group_num_start, &group_num_end, &group_num_times)) {
			if(group_num_start == group_num_end) {
				w25x_read(eeprom_info.pre_cmd, group_num_start*ACTION_SIZE, ACTION_SIZE);	
			} else {
				memset(eeprom_info.pre_cmd, 0, sizeof(eeprom_info.pre_cmd));
				strcpy((char *)eeprom_info.pre_cmd, (char *)str+1);
				eeprom_info.pre_cmd[strlen((char *)str) - 2] = '\0';
			}
			eeprom_info.pre_cmd[PRE_CMD_SIZE] = FLAG_VERIFY;
			rewrite_eeprom();
			//uart1_send_str(eeprom_info.pre_cmd);
			uart1_send_str((u8 *)"@SET PRE_CMD OK!");
		}
		return;
	}
	
	action_index = get_action_index(str);
	//<G0001#001...>
	if((action_index == -1) || str[6] != '#'){
	//if( action_index == -1 ){
		uart1_send_str("E");
		return;
	}
	//save_action_index_bak++;
	if(action_index*ACTION_SIZE % W25Q64_SECTOR_SIZE == 0)w25x_erase_sector(action_index*ACTION_SIZE/W25Q64_SECTOR_SIZE);
	replace_char(str, '<', '{');
	replace_char(str, '>', '}');
	w25x_write(str, action_index*ACTION_SIZE, strlen(str) + 1);
	//uart1_send_str(str);
	uart1_send_str("A");
	return;	
}

int get_action_index(u8 *str) {
	int index = 0;
	//uart_send_str(str);
	while(*str) {
		if(*str == 'G') {
			str++;
			while((*str != '#') && (*str != '$')) {
				index = index*10 + *str-'0';
				str++;	
			}
			return index;
		} else {
			str++;
		}
	}
	return -1;
}

void print_group(int start, int end) {
	if(start > end) {
		int_exchange(&start, &end);
	}
	for(;start<=end;start++) {
		memset(uart_receive_buf, 0, sizeof(uart_receive_buf));
		w25x_read(uart_receive_buf, start*ACTION_SIZE, ACTION_SIZE);
		uart1_send_str(uart_receive_buf);
		uart1_send_str("\r\n");
	}
}


void int_exchange(int *int1, int *int2) {
	int int_temp;
	int_temp = *int1;
	*int1 = *int2;
	*int2 = int_temp;
}

void erase_sector(int start, int end) {
	if(start > end) {
		int_exchange(&start, &end);
	}
	if(end >= 127)end = 127;
	for(;start<=end;start++) {
		SpiFlashEraseSector(start);
		sprintf(cmd_return, "@Erase %d OK!", start);
		uart1_send_str(cmd_return);
	}
	save_action_index_bak = 0;
}

//获取最大时间
int getMaxTime(u8 *str) {
   int i = 0, max_time = 0, tmp_time = 0;
   while(str[i]) {
      if(str[i] == 'T') {
          tmp_time = (str[i+1]-'0')*1000 + (str[i+2]-'0')*100 + (str[i+3]-'0')*10 + (str[i+4]-'0');
          if(tmp_time>max_time)max_time = tmp_time;
          i = i+4;
          continue;
      }
      i++;
   }
   return max_time;
}

void do_group_once(int group_num) {
	memset(uart_receive_buf, 0, sizeof(uart_receive_buf));
	//从存储芯片中读取第group_num个动作组
	w25x_read(uart_receive_buf, group_num*ACTION_SIZE, ACTION_SIZE);
	//获取最大的组时间
	action_time = getMaxTime(uart_receive_buf);	
	//把读取出来的动作组传递到parse_action执行
	parse_action(uart_receive_buf);
	sprintf((char *)cmd_return, "@DoGroup %d OK!\r\n", group_num);
	//uart1_send_str(cmd_return);
}

void loop_action(void) {
	static u32 systick_ms_bak = 0;
	if(group_do_ok == 0) {
		if(millis() - systick_ms_bak > action_time) {
			systick_ms_bak =  millis();
			if(group_num_times != 0 && do_time == 0) {
			  group_do_ok = 1;
			  uart1_send_str((u8 *)"@GroupDone!");
			  return;
			}
			//调用do_start_index个动作
			do_group_once(do_start_index);
			
			if(group_num_start<group_num_end) {
				if(do_start_index == group_num_end) {
					do_start_index = group_num_start;
					if(group_num_times != 0) {
						do_time--;
					}
					return;
				}
				do_start_index++;
			} else {
				if(do_start_index == group_num_end) {
					do_start_index = group_num_start;
					if(group_num_times != 0) {
						do_time--;
					}
					return;
				}
				do_start_index--;
			}
		}
	} else {
      action_time = 10;
	}
}

u8 check_dj_state(void) {
	int i;
	float	inc = 0;
	for(i=0;i<DJ_NUM;i++) {
		inc += duoji_doing[i].inc;
		if(inc)return 1;
	}
	return 0;
}

void parse_action(u8 *uart_receive_buf) {
	u16 index,  time, i = 0, j, step = 0;
	int bias;
	float pwm;
	float aim_temp;
	//zx_uart_send_str(uart_receive_buf);
	if(uart_receive_buf[0] == '#' && uart_receive_buf[4] == 'P' && uart_receive_buf[5] == 'S' && uart_receive_buf[6] == 'C' && uart_receive_buf[7] == 'K' && uart_receive_buf[12] == '!') {
		index = (uart_receive_buf[1] - '0')*100 + (uart_receive_buf[2] - '0')*10 + (uart_receive_buf[3] - '0');
		bias = (uart_receive_buf[9] - '0')*100 + (uart_receive_buf[10] - '0')*10 + (uart_receive_buf[11] - '0');
		if((bias >= -500) && (bias <= 500) && (index < DJ_NUM)) {
			if(uart_receive_buf[8] == '+') {
			} else if(uart_receive_buf[8] == '-') {
				bias = -bias;
			}
			aim_temp = duoji_doing[index].cur - eeprom_info.dj_bias_pwm[index] + bias;
			eeprom_info.dj_bias_pwm[index] = bias;			
			if(aim_temp > 2497){
				aim_temp = 2497;
			} else if(aim_temp < 500) {
				aim_temp = 500;
			}
			
			duoji_doing[index].aim = aim_temp;
			duoji_doing[index].inc = (duoji_doing[index].aim - duoji_doing[index].cur)/5;
			bias_systick_ms_bak = millis();
			needSaveFlag = 1;
		}
		return;
	} else if(uart_receive_buf[0] == '#' && uart_receive_buf[4] == 'P' && uart_receive_buf[5] == 'D' && uart_receive_buf[6] == 'S' && uart_receive_buf[7] == 'T' && uart_receive_buf[8] == '!') {
		index = (uart_receive_buf[1] - '0')*100 + (uart_receive_buf[2] - '0')*10 + (uart_receive_buf[3] - '0');		
		if(index < DJ_NUM) {
			duoji_doing[index].inc = 0;	
			duoji_doing[index].aim = duoji_doing[index].cur;
		}
		return;
	}
	
	step = 1;
	while(uart_receive_buf[i]) {
		if(uart_receive_buf[i] == '#' && step == 1) {
			j = i;
			index = 0;i++;
			while(uart_receive_buf[i] && uart_receive_buf[i] != 'P') {
				index = index*10 + uart_receive_buf[i]-'0';i++;
			}
			if(i-j-1 != 3) {
				step = 1;
			} else {
				step = 2;
			}
		} else if(uart_receive_buf[i] == 'P' && step == 2) {
			j = i;
			pwm = 0;i++;
			while(uart_receive_buf[i] && uart_receive_buf[i] != 'T') {
				pwm = pwm*10 + uart_receive_buf[i]-'0';i++;
			}
			if(i-j-1 != 4) {
				step = 1;
			} else {
				step = 3;
			}
		} else if(uart_receive_buf[i] == 'T' && step == 3) {
			j = i;
			time = 0;i++;
			while(uart_receive_buf[i] && uart_receive_buf[i] != '!') {
				time = time*10 + uart_receive_buf[i]-'0';i++;
			}
			
			//同步的时候防止数据太快不稳定
			if(time<500) {
				time = time+300;
			}
			
			step = 1;
			if(i-j-1 != 4) {
			} else {
				if(index < DJ_NUM && (pwm<=2500)&& (pwm>=500) && (time<=10000)) {
					pwm += eeprom_info.dj_bias_pwm[index];
					
					if(pwm>2497)pwm=2497;
					if(pwm<500)pwm=500;
					
					if(time < 20) {
						duoji_doing[index].aim = pwm;
						duoji_doing[index].cur = pwm;
						duoji_doing[index].inc = 0;
					} else {
						duoji_doing[index].aim = pwm;
						duoji_doing[index].time = time;
						duoji_doing[index].inc = (duoji_doing[index].aim -  duoji_doing[index].cur) / (duoji_doing[index].time/20.000);
					}

					//sprintf(cmd_return, "#%03dP%04dT%04d! %f \r\n", index, pwm, time, duoji_doing[index].inc);
					//uart1_send_str(cmd_return);
				} 
				
				if(index == 255) {
					for(index=0;index<DJ_NUM;index++) {
						pwm =1500 + eeprom_info.dj_bias_pwm[index];
						duoji_doing[index].aim = pwm;
						duoji_doing[index].time = time;
						duoji_doing[index].inc = (duoji_doing[index].aim -  duoji_doing[index].cur) / (duoji_doing[index].time/20.000);
					}
				}
			}
		} else {
			i++;
		}
	}	
}

void replace_char(u8*str, u8 ch1, u8 ch2) {
	while(*str) {
		if(*str == ch1) {
			*str = ch2;
		} 
		str++;
	}
	return;
}

//把eeprom_info写入到W25Q64_INFO_ADDR_SAVE_STR位置
void rewrite_eeprom(void) {
	w25x_erase_sector(W25Q64_INFO_ADDR_SAVE_STR/W25Q64_SECTOR_SIZE);
	w25x_write((u8 *)(&eeprom_info), W25Q64_INFO_ADDR_SAVE_STR, sizeof(eeprom_info_t));
}

void loop_bt_once(void) {
	static u8 first_change = 1, step = 0;
	static u32 systick_ms_bak = 0;
	if(first_change) {
		if((millis() - systick_ms_bak > 500) && (step == 0)) {
			systick_ms_bak = millis();
			uart4_init(9600);
			uart4_open();
			uart4_send_str((u8 *)"AT+BAUD8\r\n");
			step++;
		} else if((millis() - systick_ms_bak > 500) && (step == 1)){
			systick_ms_bak = millis();		
			uart4_init(115200);
			step++;
		}  else if((millis() - systick_ms_bak > 500) && (step == 2)){
			systick_ms_bak = millis();		
			uart4_send_str((u8 *)"AT+SPPNAMEZL-51-BT2.0\r\n");
			step++;
		} else if((millis() - systick_ms_bak > 500) && (step == 3)){
			systick_ms_bak = millis();
			uart4_send_str((u8 *)"AT+LENAMEZL-51-BT4.0\r\n");
			step++;
		} else if((millis() - systick_ms_bak > 500) && (step == 4)){
			systick_ms_bak = millis();
			uart4_send_str((u8 *)"AT+NAMEZL-51-BT2.0\r\n");
			step++;
			first_change = 0;
		} 
	}
}

void soft_reset(void) {
	IAP_CONTR = 0X60;
}

void car_set(int car_left, int car_right) {
	//总线马达设置	
	sprintf((char *)cmd_return, "{#006P%04dT0000!#007P%04dT0000!}", (int)(1500+car_left), (int)(1500-car_right));
	zx_uart_send_str(cmd_return);			
	return;
}


void set_servo(int index, int pwm, int time) {
	duoji_doing[index].aim = pwm;
	duoji_doing[index].time = time;
	duoji_doing[index].inc = (duoji_doing[index].aim -  duoji_doing[index].cur) / (duoji_doing[index].time/20.000);
	sprintf((char *)cmd_return, "#%03dP%04dT%04d!(%.1f)\r\n", index, pwm, time, kinematics.servo_angle[index]);
	uart1_send_str(cmd_return);	
}



int kinematics_move(float x, float y, float z, int time) {
	int i,j, min = 0, flag = 0;
	
	if(y < 0)return 0;
	//寻找最佳角度
	flag = 0;
	for(i=0;i>=-135;i--) {
		if(0 == kinematics_analysis(x,y,z,i,&kinematics)){
			min = i;
			flag = 1;
			break;
			//if(i<min)min = i;
			//flag = 1;
		}
	}
	
	//用3号舵机与水平最大的夹角作为最佳值
	if(flag) {
		kinematics_analysis(x,y,z,min,&kinematics);
		for(j=1;j<4;j++) {
			set_servo(j, kinematics.servo_pwm[j], time);
		}
		return 1;
	}
	
	return 0;
}




