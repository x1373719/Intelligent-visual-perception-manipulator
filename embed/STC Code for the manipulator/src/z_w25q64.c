#include "z_w25q64.h"
#include <intrins.h>
/****************************************************************************
	*	@笔者	：	W
	*	@日期	：	2019年12月02日
	*	@所属	：	杭州众灵科技
	*	@论坛	：	www.ZL-robot.com
	*	@功能	：	存放FLASH相关的函数
	*	@函数列表：
	*	1.	void	SpiFlashInit(void); -- 初始化SPI
	*	2.	void	SpiSetSpeedLow(void); -- SPI设置为低时钟频率 CPU_CLK/128
	*	3.	void	SpiSetSpeedHigh(void); -- SPI设置为高时钟频率 CPU_CLK/4
	*	4.	u8		SpiWriteRead(u8 d); -- SPI读写数据
	*	5.	u16		SpiFlashReadID(void); -- 读取FLASH ID
	*	6.	u8		SpiFlashReadSR(void); -- 读取状态寄存器 
	*	7.	void	SpiFlashWriteSR(u8 sr); -- 写状态寄存器
	*	8.	void	SpiFlashWriteEnable(void); -- 写使能 
	*	9.	void	SpiFlashWriteDisable(void); -- 写禁止
	*	10.	char	SpiFlashReadChar(u32 ReadAddr); -- 读取flash
	*	11.	void	SpiFlashRead(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead); -- 读取flash
	*	12.	void	SpiFlashWrite(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite); -- 写入flash
	*	13.	void	SpiFlashWriteChar(char tmp,u32 WriteAddr); -- 单字节写入flash
	*	14.	void	SpiFlashWritePage(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite); -- 单页写入flash
	*	15.	void	SpiFlashWriteSector(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite); -- 单扇区写入flash
	*	16.	void	SpiFlashWriteNoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite); -- 无检验写入flash
	*	17.	void	SpiFlashEraseSector(u32 Dst_Addr); -- 扇区擦除
	*	18.	void	SpiFlashEraseChip(void); -- 整片擦除
	*	19.	void	SpiFlashWaitBusy(void); -- 等待空闲
	*	20.	void	SpiFlashPowerDown(void); -- 进入掉电模式
	*	21.	void	SpiFlashWakeUp(void); -- 唤醒
 ****************************************************************************/

#define  PIN0 0x01	//0000,0001
#define  PIN1 0x02	//0000,0010
#define  PIN2 0x04	//0000,0100
#define  PIN3 0x08	//0000,1000
#define  PIN4 0x10	//0001,0000
#define  PIN5 0x20	//0010,0000
#define  PIN6 0x40	//0100,0000
#define  PIN7 0x80	//1000,0000

void _delay_us(u16 t) {
	while(t--) {
		_nop_();
		_nop_();
		_nop_();
	}
}


/***********************************************
	函数名称：SpiFlashInit() 
	功能介绍：初始化SPI FLASH的IO口
	函数参数：无
	返回值：	无
 ***********************************************/
void SpiFlashInit(void) {	 
	P2M1|=  PIN2 ;
	P2M0&=~(PIN2);
	P2M1&=~((PIN1)|(PIN3)|(PIN4));
	P2M0|= ((PIN1)|(PIN3)|(PIN4));
	
	P_SW1 &= SPI_S1_1;										//CS P2.4 SCLK P2.1
	P_SW1 |= SPI_S1_2;										//MOSI P2.3 MISO P2.2	
	SPDAT  = 0;         									//清空数据寄存器
	SPSTAT = SPIF|WCOL; 									//清空SPI状态寄存器
	SPCTL  = SPEN|MSTR|SP_CLK0|SSIG;			//SPI设置为主机模式
} 

/***********************************************
	函数名称：SpiSetSpeedLow() 
	功能介绍：SPI设置为低时钟频率 CPU_CLK/128
	函数参数：无
	返回值：	无
 ***********************************************/
void SpiSetSpeedLow(void) {
	SPCTL  &=~SP_CLK3; 
	SPCTL |=SP_CLK3;
}

/***********************************************
	函数名称：SpiSetSpeedHigh() 
	功能介绍：SPI设置为高时钟频率 CPU_CLK/4
	函数参数：无
	返回值：	无
 ***********************************************/
void SpiSetSpeedHigh(void) {
	SPCTL  &=~SP_CLK3; 
	SPCTL  |=SP_CLK0;
}

/***********************************************
	函数名称：SpiWriteRead() 
	功能介绍：SPI读写数据
	函数参数：d 发送的数据
	返回值：	SPDAT 返回字节数据
 ***********************************************/
u8 SpiWriteRead(u8 d) {
	SPDAT = d;                  					//触发SPI发送数据
	while (!(SPSTAT & SPIF));   					//等待发送完成
	SPSTAT = SPIF | WCOL;       					//清除SPI状态位
	return SPDAT;               					//返回SPI数据
}

/***********************************************
	函数名称：SpiFlashReadID() 
	功能介绍：读取芯片ID
	函数参数：无
	返回值：	0XEF13,表示芯片型号为W25Q80
						0XEF14,表示芯片型号为W25Q16
						0XEF15,表示芯片型号为W25Q32
						0XEF16,表示芯片型号为W25Q64
 ***********************************************/
u16 SpiFlashReadID(void) {
	u16 Temp = 0;
	SPI_FLASH_CS(0);
	SpiWriteRead(W25X_ManufactDeviceID);	//发送读取ID命令
	SpiWriteRead(0x00);
	SpiWriteRead(0x00);
	SpiWriteRead(0x00);
	Temp|=SpiWriteRead(0xFF)<<8;
	Temp|=SpiWriteRead(0xFF);
	SPI_FLASH_CS(1);
	return Temp;
}

/***********************************************
	函数名称：SpiFlashReadSR() 
	功能介绍：读SPI_FLASH的状态寄存器
	函数参数：无
	返回值：	读取的SPI_FLASH的状态寄存器值
						BIT		7		6		5		4		3		2		1		0
									SPR	RV	TB	BP2	BP1	BP0	WEL	BUSY
						SPR:默认0,状态寄存器保护位,配合WP使用
						TB,BP2,BP1,BP0:FLASH区域写保护设置
						WEL:写使能锁定
						BUSY:忙标记位(1：忙	0：空闲)
 ***********************************************/
u8 SpiFlashReadSR(void) {  
	u8 rt=0;   
	SPI_FLASH_CS(0);                      //使能器件   
	SpiWriteRead(W25X_ReadStatusReg);    	//发送读取状态寄存器命令    
	rt=SpiWriteRead(0Xff);               	//读取一个字节  
	SPI_FLASH_CS(1);                      //取消片选     
	return rt;   
}

/***********************************************
	函数名称：SpiFlashWriteSR() 
	功能介绍：写SPI_FLASH的状态寄存器
	函数参数：写入的SPI_FLASH的状态寄存器值
	返回值：	无
						BIT		7		6		5		4		3		2		1		0
									SPR	RV	TB	BP2	BP1	BP0	WEL	BUSY
						只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写
 ***********************************************/
void SpiFlashWriteSR(u8 sr) {   
	SPI_FLASH_CS(0);                      //使能器件   
	SpiWriteRead(W25X_WriteStatusReg);   	//发送写取状态寄存器命令    
	SpiWriteRead(sr);                    	//写入一个字节  
	SPI_FLASH_CS(1);                      //取消片选     	      
}   

/***********************************************
	函数名称：SpiFlashWriteEnable() 
	功能介绍：SPI_FLASH写使能，WEL置位
	函数参数：无
	返回值：	无
 ***********************************************/
void SpiFlashWriteEnable(void) {
	SPI_FLASH_CS(0);                      //使能器件   
	SpiWriteRead(W25X_WriteEnable);       //发送写使能  
	SPI_FLASH_CS(1);                      //取消片选     	      
} 

/***********************************************
	函数名称：SpiFlashWriteDisable() 
	功能介绍：SPI_FLASH写禁止，将WEL清零
	函数参数：无
	返回值：	无
 ***********************************************/
void SpiFlashWriteDisable(void) {  
	SPI_FLASH_CS(0);                      //使能器件   
  SpiWriteRead(W25X_WriteDisable);      //发送写禁止指令    
	SPI_FLASH_CS(1);                      //取消片选     	      
} 	

/***********************************************
	函数名称：SpiFlashRead() 
	功能介绍：SPI读取1个字节的数据
	函数参数：ReadAddr       -开始读取的地址(24bit)
	返回值：	无
 ***********************************************/
char SpiFlashReadChar(u32 ReadAddr) { 
	static char tmp;
	SPI_FLASH_CS(0);											//使能器件   
  SpiWriteRead(W25X_ReadData);					//发送读取命令   
  SpiWriteRead((u8)((ReadAddr)>>16));		//发送24bit地址    
  SpiWriteRead((u8)((ReadAddr)>>8));	
  SpiWriteRead((u8)(ReadAddr&0xFF));   
	tmp=SpiWriteRead(0XFF);				//读数  
	SPI_FLASH_CS(1);
	return tmp;
}  

/***********************************************
	函数名称：SpiFlashRead() 
	功能介绍：读SPI_FLASH
	函数参数：pBuffer        -数据存储区
						ReadAddr       -开始读取的地址(24bit)
						NumByteToRead  -要读取的字节数(最大65535)
	返回值：	无
 ***********************************************/
void SpiFlashRead(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead) { 
	u16 i;
	SPI_FLASH_CS(0);											//使能器件   
  SpiWriteRead(W25X_ReadData);					//发送读取命令   
  SpiWriteRead((u8)((ReadAddr)>>16));		//发送24bit地址    
  SpiWriteRead((u8)((ReadAddr)>>8));	
  SpiWriteRead((u8)(ReadAddr&0xFF));   
  for(i=0;i<NumByteToRead;i++){ 
		pBuffer[i]=SpiWriteRead(0XFF);			//循环读数  
  }
	SPI_FLASH_CS(1);  				    	      
}  

/***********************************************
	函数名称：SpiFlashWrite() 
	功能介绍：写SPI_FLASH
	函数参数：pBuffer        -数据存储区
						WriteAddr      -开始写入的地址(24bit)
						NumByteToWrite -要读取的字节数(最大65535)
	返回值：	无
 ***********************************************/
void SpiFlashWrite(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 sector = 0;
	u16 sectorremain;
	
	sectorremain=4096-WriteAddr%4096; 		//单扇区剩余的字节数		 	    
	if(NumByteToWrite<=sectorremain)
		sectorremain=NumByteToWrite;
	while(1){	   
		sector=WriteAddr>>12;								//扇区地址 
		SpiFlashEraseSector(sector);				//擦除这个扇区
		SpiFlashWriteSector(pBuffer,WriteAddr,sectorremain);
		if(NumByteToWrite==sectorremain)
			break;														//写入结束了
	 	else{
			pBuffer 				+= sectorremain;
			WriteAddr 			+= sectorremain;	
			NumByteToWrite	-= sectorremain;	//减去已经写入了的字节数
			if(NumByteToWrite>4096)
				sectorremain=4096; 							//一次可以写入4096个字节
			else 
				sectorremain=NumByteToWrite; 	   //不够4096个字节了
		}
	}	    
}

/***********************************************
	函数名称：SpiFlashWriteChar() 
	功能介绍：SPI写入1个字节的数据
	函数参数：tmp        		 -数据
						WriteAddr      -开始写入的地址(24bit)
	返回值：	无
 ***********************************************/
void SpiFlashWriteChar(char tmp,u32 WriteAddr) {
	SpiFlashWriteEnable();
	SPI_FLASH_CS(0);											//使能器件
  SpiWriteRead(W25X_PageProgram);				//发送写页命令   
  SpiWriteRead((u8)((WriteAddr)>>16));	//发送24bit地址    
  SpiWriteRead((u8)((WriteAddr)>>8));   
  SpiWriteRead((u8)WriteAddr);
	SpiWriteRead(tmp);										//写数  
	SPI_FLASH_CS(1);											//取消片选 
	SpiFlashWaitBusy();					          //等待写入结束
	//uart1_send_str("Byte has been written.\r\n");
} 

/***********************************************
	函数名称：SpiFlashWritePage() 
	功能介绍：SPI在一页内写入少于256个字节的数据
	函数参数：pBuffer        -数据存储区
						WriteAddr      -开始写入的地址(24bit)
						NumByteToWrite -要写入的字节数(最大256),该数不应该超过该页的剩余字节数
	返回值：	无
 ***********************************************/
void SpiFlashWritePage(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite) {
 	u16 pageremain, i;	
	pageremain=256-WriteAddr%256; 				//单页剩余的字节数		 	    
	if(NumByteToWrite>pageremain){
		NumByteToWrite=pageremain;
		//uart1_send_str((u8 *)"There are not enough writable bytes left in this page!\r\n");
	}
  SpiFlashWriteEnable();              	//写使能 
	SPI_FLASH_CS(0);											//使能器件
  SpiWriteRead(W25X_PageProgram);				//发送写页命令   
  SpiWriteRead((u8)((WriteAddr)>>16));	//发送24bit地址    
  SpiWriteRead((u8)((WriteAddr)>>8));   
  SpiWriteRead((u8)WriteAddr);

/*******全局变量外部声明*******/
  for(i=0;i<NumByteToWrite;i++){ 
		SpiWriteRead(pBuffer[i]);						//循环写数  
  }
	SPI_FLASH_CS(1);											//取消片选 
	SpiFlashWaitBusy();					          //等待写入结束
	//uart1_send_int(NumByteToWrite);
	//uart1_send_str(" bytes have been written.\r\n");
} 

/***********************************************
	函数名称：SpiFlashWriteSector() 
	功能介绍：SPI在一扇区内写入少于4096个字节的数据
	函数参数：pBuffer        -数据存储区
						WriteAddr      -开始写入的地址(24bit)
						NumByteToWrite -要写入的字节数(最大4096),该数不应该超过该扇区的剩余字节数
	返回值：	无
 ***********************************************/
void SpiFlashWriteSector(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite) {
 	u16 sectorremain;	   
	sectorremain=4096-WriteAddr%4096; 				//单扇区剩余的字节数		 	    
	if(NumByteToWrite>sectorremain){
		NumByteToWrite=sectorremain;
		//uart1_send_str((u8 *)"There are not enough writable bytes left in this sector!\r\n");
	}
	SpiFlashWriteNoCheck(pBuffer,WriteAddr,NumByteToWrite);
} 

/***********************************************
	函数名称：SpiFlashWriteNoCheck() 
	功能介绍：无检验写SPI_FLASH
	函数参数：pBuffer        -数据存储区
						WriteAddr      -开始写入的地址(24bit)
						NumByteToWrite -要写入的字节数(最大65535 1块 16扇区 256页)
	返回值：	无
 ***********************************************/
void SpiFlashWriteNoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite) { 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; 				//单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)
		pageremain=NumByteToWrite;
	while(1){	   
		SpiFlashWritePage(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)
			break;														//写入结束了
	 	else{
			pBuffer 				+= pageremain;
			WriteAddr 			+= pageremain;	
			NumByteToWrite	-= pageremain;		//减去已经写入了的字节数
			if(NumByteToWrite>256)
				pageremain=256; 								//一次可以写入256个字节
			else 
				pageremain=NumByteToWrite; 	    //不够256个字节了
		}
	}	    
} 

/***********************************************
	函数名称：SpiFlashEraseSector() 
	功能介绍：擦除一个扇区 最少150毫秒
	函数参数：Dst_Addr  -扇区地址 (最大2047)
	返回值：	无
 ***********************************************/
void SpiFlashEraseSector(u32 Dst_Addr) {  
	Dst_Addr <<= 12;
	SpiFlashWriteEnable();                //SET WEL 	 
	SpiFlashWaitBusy();   
	SPI_FLASH_CS(0);                      //使能器件   
	SpiWriteRead(W25X_SectorErase);       //发送扇区擦除指令 
	SpiWriteRead((u8)((Dst_Addr)>>16));  	//发送24bit地址    
	SpiWriteRead((u8)((Dst_Addr)>>8));   
	SpiWriteRead((u8)Dst_Addr);  
	SPI_FLASH_CS(1);                      //取消片选     	      
	SpiFlashWaitBusy();   				       	//等待擦除完成
	//uart1_send_str("Sector has been erased.\r\n");
}

/***********************************************
	函数名称：SpiFlashEraseChip() 
	功能介绍：擦除整个芯片
	函数参数：无
	返回值：	无
 ***********************************************/
void SpiFlashEraseChip(void) {                                   
	SpiFlashWriteEnable();             		//SET WEL 
	SpiFlashWaitBusy();   
	SPI_FLASH_CS(0);                      //使能器件   
	SpiWriteRead(W25X_ChipErase);        	//发送片擦除命令  
	SPI_FLASH_CS(1);                      //取消片选     	      
	SpiFlashWaitBusy();   				      	//等待芯片擦除结束
	//uart1_send_str("Chip has been erased.\r\n");
} 

/***********************************************
	函数名称：SpiFlashWaitBusy() 
	功能介绍：等待空闲
	函数参数：无
	返回值：	无
 ***********************************************/
void SpiFlashWaitBusy(void) {   
	while((SpiFlashReadSR()&0x01)==0x01);  // 等待BUSY位清空
}  

/***********************************************
	函数名称：SpiFlashPowerDown() 
	功能介绍：进入掉电模式
	函数参数：无
	返回值：	无
 ***********************************************/
void SpiFlashPowerDown(void) { 
	SPI_FLASH_CS(0);                       //使能器件   
	SpiWriteRead(W25X_PowerDown);          //发送掉电命令  
	SPI_FLASH_CS(1);                       //取消片选     	      
	_delay_us(3);                           //等待TPD  
}

/***********************************************
	函数名称：SpiFlashWakeUp() 
	功能介绍：唤醒
	函数参数：无
	返回值：	无
 ***********************************************/
void SpiFlashWakeUp(void) {  
	SPI_FLASH_CS(0);												//使能器件   
	SpiWriteRead(W25X_ReleasePowerDown);   	//发送唤醒指令 
	SPI_FLASH_CS(1);                        //取消片选     	      
	_delay_us(3);                            //等待TRES1
}























