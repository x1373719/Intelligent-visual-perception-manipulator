#ifndef __W25Q64_H__
#define __W25Q64_H__

/*

W25Q64是华邦公司推出的大容量SPI FLASH产品，其容量为64Mb。
该25Q系列的器件在灵活性和性能方面远远超过普通的串行闪存器件。
W25Q64将8M字节的容量分为128个块，每个块大小为64K字节，每个块又分为16个扇区，每个扇区4K个字节。
W25Q64的最小擦除单位为一个扇区，也就是每次必须擦除4K个字节。所以，这需要给W25Q64开辟一个至少4K的缓存区，这样必须要求芯片有4K以上的SRAM才能有很好的操作。 
W25Q64的擦写周期多达10W次，可将数据保存达20年之久，支持2.7~3.6V的电压，支持标准的SPI，还支持双输出/四输出的SPI，最大SPI时钟可达80Mhz。

*/

#include "z_stc15.h"

/*******SPI快捷指令表*******/
#define	SPI_FLASH_CS(x)			P24=(x)

/*******对SPI管脚切换宏定义*******/
#define SPI_S0		0xf3							//P_SW1.3 P_SW1.2 &=
#define SPI_S1_1	0xf7							//P_SW1.3	&=
#define SPI_S1_2	0x04							//P_SW1.2 |=
#define SPI_S2_1	0x08							//P_SW1.3	|=
#define SPI_S2_2	0xfb							//P_SW1.2 &=

/*******对SPCTL寄存器的设置位宏定义*******/
#define SSIG    0x80  							//SS引脚忽略
#define SPEN    0x40  							//SPI使能位
#define DORD    0x20  							//SPI数据发送LSB最先发送
#define MSTR    0x10  							//主从模式选择
#define CPOL    0x08  							//SPICLK空闲时为高电平
#define CPHA    0x04  							//数据在SPICLK的前时钟沿驱动,并在后时钟沿采样
#define SP_CLK0 0x00  							//SPI时钟频率为CPU_CLK/4
#define SP_CLK1 0x01  							//SPI时钟频率为CPU_CLK/16
#define SP_CLK2 0x02  							//SPI时钟频率为CPU_CLK/64
#define SP_CLK3 0x03  							//SPI时钟频率为CPU_CLK/128

/*******对SPSTAT寄存器的设置位宏定义*******/
#define SPIF    0x80  //传输完成标志
#define WCOL    0x40  //SPI写冲突标志		

/*******W25Q系列芯片ID*******/
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16

/*******W25Q64芯片变量宏定义*******/
#define W25Q64_SECTOR_SIZE	4096		//4K
#define W25Q64_SECTOR_NUM		2048		//8*1024/4 = 2048

/*******W25Q64芯片地址存储表*******/
#define FLASH_ASC16_ADDRESS                 0
#define FLASH_HZK16_ADDRESS                 0x1000

#define FLASH_SYSTEM_CONFIG_ADDRESS         0x43000

#define FLASH_BITMAP1_SIZE_ADDRESS	        0x50000
#define FLASH_BITMAP2_SIZE_ADDRESS	        FLASH_BITMAP1_SIZE_ADDRESS+0x28000
#define FLASH_BITMAP3_SIZE_ADDRESS	        FLASH_BITMAP2_SIZE_ADDRESS+0x28000
#define FLASH_BITMAP4_SIZE_ADDRESS	        FLASH_BITMAP3_SIZE_ADDRESS+0x28000
#define FLASH_BITMAP5_SIZE_ADDRESS	        FLASH_BITMAP4_SIZE_ADDRESS+0x28000	
#define FLASH_BITMAP6_SIZE_ADDRESS	        FLASH_BITMAP5_SIZE_ADDRESS+0x28000

#define FLASH_BITMAPMAIN_SIZE_ADDRESS       FLASH_BITMAP6_SIZE_ADDRESS+0x28000
#define FLASH_BITMAPDS1302_SIZE_ADDRESS     FLASH_BITMAPMAIN_SIZE_ADDRESS+0x28000
#define FLASH_BITMAPDS18B20_SIZE_ADDRESS    FLASH_BITMAPDS1302_SIZE_ADDRESS+0x28000
#define FLASH_BITMAPBLUETOOTH_SIZE_ADDRESS  FLASH_BITMAPDS18B20_SIZE_ADDRESS+0x28000

/*******FLASH相关指令表*******/
#define W25X_WriteEnable			0x06 
#define W25X_WriteDisable			0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData					0x03 
#define W25X_FastReadData			0x0B 
#define W25X_FastReadDual			0x3B 
#define W25X_PageProgram			0x02 
#define W25X_SectorErase			0x20 
#define W25X_BlockErase				0xD8 
#define W25X_ChipErase				0xC7 
#define W25X_PowerDown				0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID					0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

/*******FLASH相关函数声明*******/
void	SpiFlashInit(void);
void	SpiSetSpeedLow(void);
void	SpiSetSpeedHigh(void);
u8		SpiWriteRead(u8 d);
u16		SpiFlashReadID(void);																									//读取FLASH ID
u8		SpiFlashReadSR(void);																									//读取状态寄存器 
void 	SpiFlashWriteSR(u8 sr);																								//写状态寄存器
void 	SpiFlashWriteEnable(void);																						//写使能 
void 	SpiFlashWriteDisable(void);																						//写禁止
char 	SpiFlashReadChar(u32 ReadAddr);																				//读取flash
void 	SpiFlashRead(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);							//读取flash
void 	SpiFlashWrite(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);					//写入flash
void 	SpiFlashWriteChar(char tmp,u32 WriteAddr);														//单字节写入flash
void 	SpiFlashWritePage(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);			//单页写入flash
void 	SpiFlashWriteSector(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);		//单扇区写入flash
void 	SpiFlashWriteNoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);		//无检验写入flash
void 	SpiFlashEraseSector(u32 Dst_Addr);																		//扇区擦除
void 	SpiFlashEraseChip(void);																							//整片擦除
void 	SpiFlashWaitBusy(void);																							//等待空闲
void 	SpiFlashPowerDown(void);																							//进入掉电模式
void	SpiFlashWakeUp(void);	

#define w25x_init() SpiFlashInit()
#define w25x_readId()	SpiFlashReadID()
#define w25x_read(buf, addr, len) SpiFlashRead(buf, addr, len)
#define w25x_write(buf, addr, len) SpiFlashWriteNoCheck(buf, addr, len)
#define w25x_erase_sector(addr) SpiFlashEraseSector(addr)

#endif