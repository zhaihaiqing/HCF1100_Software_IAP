/* Includes ------------------------------------------------------------------*/
#include "main.h"



iapfun jump2app; //定义一个函数类型

//设置栈顶指针
//addr:栈顶地址
__asm void MSR_MSP(unsigned int addr) 
{
    MSR MSP, r0 		//set Main Stack value
    BX r14
}

/*******************************************************************************
* Function Name  : iap_load_app
* Description    : 跳转函数，用于跳转至APP程序区运行。
* Input          : APP代码起始地址
* Output         : None
* Return         : None
*******************************************************************************/
void iap_load_app(unsigned int appxaddr)
{
	if(((*(volatile unsigned int*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		jump2app=(iapfun)*(volatile unsigned int*)(appxaddr+4);					//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(volatile unsigned int*)appxaddr);								//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();																//跳转到APP.
	}
}	

/*******************************************************************************
* Function Name  : ClearUsartBuf
* Description    : 清除串口缓存
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void ClearUsartBuf(void)
{
	USART1_ClearBuf_Flag();			//清空串口接收缓存
	ModbusDataPackage.DataLen = 0;  //先清空长度，注意清空顺序
	ModbusDataPackage.DataFlag = 0; //清空标记位
}

/*******************************************************************************
* Function Name  : Re_BootLoader_Info
* Description    : 回复查询BootLoader信息
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void Re_BootLoader_Info(void)
{
	uint16_t M_crc=0;
	uint8_t	tem[8]={0};
	
	tem[0]=BL_Data.DeviceAddress;
	tem[1]=0xA2;
	tem[2]=(BL_Data.DeviceType&0xff00)>>8;
	tem[3]= BL_Data.DeviceType&0xff;
	tem[4]=(BL_Data.SoftwareVersion&0xff00)>>8;
	tem[5]= BL_Data.SoftwareVersion&0xff;
	M_crc=CRC16_Check(tem,6);
	tem[6] = (M_crc & 0xff);//校验低8位
	tem[7] = (M_crc >> 8);//校验高8位
	
	U485SendData(tem,8);//向485发送数据
	
	ClearUsartBuf();
}

/*******************************************************************************
* Function Name  : Re_BootLoader_Info
* Description    : 回复升级信息
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void Re_BootLoader_Update(void)
{
	uint16_t M_crc=0;
	uint8_t	tem[8]={0};
	
	tem[0]=0x00;tem[1]=0x00;tem[2]=0x00;tem[3]=0x00;
	EEWrite(Update_Firmware_Flag_Addr,tem,4);				//将固件更新完成标志位改为0x08080808
	Update_Firmware_Flag=0x00;
				
	tem[0]=BL_Data.DeviceAddress;
	tem[1]=0xA3;
	tem[2]=0x44;
	tem[3]=0x66;
	tem[4]=0x88;
	tem[5]=0xaa;
	M_crc=CRC16_Check(tem,6);
	tem[6] = (M_crc & 0xff);//校验低8位
	tem[7] = (M_crc >> 8);//校验高8位
	U485SendData(tem,8);//向485发送数据
	
	ClearUsartBuf();
}


uint8_t Wait_Commend(void)
{
	uint16_t count=0,i=0,M_crc=0;
	
	count=1000;
	log_info("PowerON Wait Update Firmware 5S...\r\n");
	while(1)
	{
		Delay(5);
		ClC_WatchDogTask();//等待MT消息
		count--;
		//if(count==800)log_info("PowerON Wait Update Firmware 4S...\r\n");
		//if(count==600)log_info("PowerON Wait Update Firmware 3S...\r\n");
		//if(count==400)log_info("PowerON Wait Update Firmware 2S...\r\n");
		//if(count==200)log_info("PowerON Wait Update Firmware 1S...\r\n");
		if(count==0)
		{
			log_info("Wait_Update Timeout!\r\n");
			return 0;
		}
		
		if(ModbusDataPackage.DataFlag)
		{
			for(i=0;i<ModbusDataPackage.DataLen;i++)
			{
				ModbusDataPackage.dat[i] =USART1_GetChar();//将串口数据放到指定buf
			}
			
			M_crc = CRC16_Check((uint8_t *)ModbusDataPackage.dat,ModbusDataPackage.DataLen-2 );
			if( (( M_crc == ( (ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1] << 8) |    ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2])        )) || \
				(((ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1]) == 0xff    ) && ((ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2]) == 0xff))  )				
			{
			
				if((ModbusDataPackage.dat[0]==BL_Data.DeviceAddress)&&(ModbusDataPackage.DataLen==8))
				{
					if(ModbusDataPackage.dat[1]==0x22)Re_BootLoader_Info();
					if(ModbusDataPackage.dat[1]==0x23){Re_BootLoader_Update();return 0;};
				}
			}
			ClearUsartBuf();
		}
	}
}

/*******************************************************************************
* Function Name  : Update_Firmware
* Description    : 固件更新函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

unsigned char Update_Firmware(void)
{
	unsigned char tem[12]={0};
	unsigned int databuf[BYTES_NUM_EACH_TRAN/4]={0};
	unsigned short i=0,j=0,M_crc=0,count=0;

	//固件升级需要三步骤
	//第一步，等待握手信息,等待时长10s
	log_info("\r\nStep 1 :Wait MT ConnectInfo!\r\n");
	while(1)
	{
		i=1000;
		ClC_WatchDogTask();//等待MT消息
		while(!ModbusDataPackage.DataFlag)
		{
			Delay(10);
			ClC_WatchDogTask();//等待MT消息
			i--;
			if(i==0)
			{
				log_info("\r\nStep 1 :Wait MT Connect Timeout!\r\n");
				return 0;
			}
		}
		for(i=0;i<ModbusDataPackage.DataLen;i++)
		{
			ModbusDataPackage.dat[i] =USART1_GetChar();//将串口数据放到指定buf
		}
		M_crc = CRC16_Check((uint8_t *)ModbusDataPackage.dat,ModbusDataPackage.DataLen-2 );
		if( (( M_crc == ( (ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1] << 8) |    ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2])        )) || \
			(((ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1]) == 0xff    ) && ((ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2]) == 0xff))  )				
		{
			if( (ModbusDataPackage.dat[0]==BL_Data.DeviceAddress)&&(ModbusDataPackage.DataLen==8) && (ModbusDataPackage.dat[1]==0x25)  ) 
			{
				log_info("ConnectDevice OK!\r\n");
				tem[0]=BL_Data.DeviceAddress;
				tem[1]=0xA5;
				tem[2]=0x44;
				tem[3]=0x66;
				tem[4]=0x88;
				tem[5]=0xaa;
				M_crc=CRC16_Check(tem,6);
				tem[6] = (M_crc & 0xff);//校验低8位
				tem[7] = (M_crc >> 8);//校验高8位
				U485SendData(tem,8);//向485发送数据
				
				ClearUsartBuf();
				break;	//跳出当前while(1)
			}
			else if( (ModbusDataPackage.dat[0]==BL_Data.DeviceAddress)&&(ModbusDataPackage.DataLen==8) && (ModbusDataPackage.dat[1]==0x22) )
			{
				Re_BootLoader_Info();
			}
			else 
			{
				ClearUsartBuf();
				//ModbusReturnAckInfo(02)//向总线发送错误
			}
		}
		ClearUsartBuf();
		
	}
	
	//第二步，等待主机发送APP镜像信息,等待时长10s
	log_info("\r\nStep 2 :Wait MT APPImageInfo!\r\n");
	while(1)
	{
		i=1000;
		while(!ModbusDataPackage.DataFlag)
		{
			Delay(10);
			ClC_WatchDogTask();//等待MT消息
			i--;
			if(i==0)
			{
				log_info("\r\nStep 2 :Wait MT APPImageInfo Timeout!\r\n");
				return 0;
			}
		}
		//log_info("SysTick_Count:%d\r\n",SysTick_Count);
		for(i=0;i<ModbusDataPackage.DataLen;i++)
		{
			ModbusDataPackage.dat[i] =USART1_GetChar();//将串口数据放到指定buf
		}
		M_crc = CRC16_Check((uint8_t *)ModbusDataPackage.dat,ModbusDataPackage.DataLen-2 );
		if( (( M_crc == ( (ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1] << 8) |    ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2])        )) || \
			(((ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1]) == 0xff    ) && ((ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2]) == 0xff))  )				
		{
			if( (ModbusDataPackage.dat[0]==BL_Data.DeviceAddress)&&(ModbusDataPackage.DataLen==12) && (ModbusDataPackage.dat[1]==0x26)  ) 
			{
				BL_Data.Program_Version = (ModbusDataPackage.dat[2]<<8) | ModbusDataPackage.dat[3];
				BL_Data.Program_Size    = (ModbusDataPackage.dat[4]<<24) | (ModbusDataPackage.dat[5]<<16)| \
										  (ModbusDataPackage.dat[6]<<8) | ModbusDataPackage.dat[7];	
				BL_Data.Totle_Frame		=  ModbusDataPackage.dat[8]<<8 | ModbusDataPackage.dat[9];
				BL_Data.Totle_Frame_Count=	BL_Data.Totle_Frame;
	
				log_info("App Info-Program_Version:0x%x ,Program_Size:%d,Totle_Frame:%d\r\n",BL_Data.Program_Version,BL_Data.Program_Size,BL_Data.Totle_Frame);
	
				log_info("CPU Erage Flash Now . . .\r\n");
				BL_Data.End_sector=BL_Data.Program_Size/4 + BL_Data.Start_sector;	//计算APP程序最后一位所在扇区号
				for(i=BL_Data.Start_sector;i<=BL_Data.End_sector;i++)				//循环擦除APP程序将要占用的扇区
					for(j=BL_Data.Start_Page;j<=FLASH_PAGE_NUM_EACH_SECTOR;j++)
						FLASH_Page_Erase(i,j);										//擦除Flash
				log_info("Flash  Erage  OK . . .\r\n");
				log_info("Program_Version:%d.%d.%d Program_Size:%dKb Totle_Frame:%d\r\n",(BL_Data.Program_Version&0xff00)>>8,\
						(BL_Data.Program_Version&0xff)/10,(BL_Data.Program_Version&0xff)%10,BL_Data.Program_Size,BL_Data.Totle_Frame);
				
				tem[0]=BL_Data.DeviceAddress;
				tem[1]=0xA6;
				tem[2]=0x44;
				tem[3]=0x66;
				tem[4]=0x88;
				tem[5]=0xaa;
				M_crc=CRC16_Check(tem,6);
				tem[6] = (M_crc & 0xff);//校验低8位
				tem[7] = (M_crc >> 8);	//校验高8位
				U485SendData(tem,8);	//向485发送数据
				
				//log_info("SysTick_Count:%d\r\n",SysTick_Count);
				break;
			}
			else 
			{
				ClearUsartBuf();
				//ModbusReturnAckInfo(02)//向总线发送错误
			}
		}
		ClearUsartBuf();
	}
	
	//第三步，循环接收镜像数据，每次等待时长10s，超时退出
	log_info("\r\nStep 3 :Wait MT APPData!\r\n");
	ClearUsartBuf();
	while(1)
	{
		i=1000;
		while(!ModbusDataPackage.DataFlag)
		{
			Delay(10);
			ClC_WatchDogTask();//等待MT消息
			i--;
			if(i==0)
			{
				log_info("\r\nStep 3 :Wait MT APPData Timeout!\r\n");
				return 0;
			}
		}
		//log_info("SysTick_Count:%d\r\n",SysTick_Count);
		for(i=0;i<ModbusDataPackage.DataLen;i++)
		{
			ModbusDataPackage.dat[i] =USART1_GetChar();//将串口数据放到指定buf
		}
		M_crc = CRC16_Check((uint8_t *)ModbusDataPackage.dat,ModbusDataPackage.DataLen-2 );
		if( (( M_crc == ( (ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1] << 8) |    ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2])        )) || \
			(((ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1]) == 0xff    ) && ((ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2]) == 0xff))  )				
		{
			if( (ModbusDataPackage.dat[0]==BL_Data.DeviceAddress)&&(ModbusDataPackage.DataLen==262) && (ModbusDataPackage.dat[1]==0x27)  ) 
			{
				//数据整合，存flash,最后一条数据，不足整包的补0,注意大小端格式
				memset(databuf,0,BYTES_NUM_EACH_TRAN/4);//清除缓存
				for(i=0;i<BYTES_NUM_EACH_TRAN/4;i++)	//将字节据拼成字（4B）
				{
					databuf[i]=(ModbusDataPackage.dat[i*4+7]<<24) | (ModbusDataPackage.dat[i*4+6]<<16) | (ModbusDataPackage.dat[i*4+5]<<8) | ModbusDataPackage.dat[i*4+4];
					//log_info("databuf[%d]:0x%x\r\n",i,databuf[i]);
				}
				count++;
				log_info("Write to Flash %d. . .\r\n",count);
				Write_4NByte_to_Flash(BL_Data.APP_FlashAddr,databuf,BYTES_NUM_EACH_TRAN/4);	//向Flash中写入数据
				BL_Data.APP_FlashAddr = BL_Data.APP_FlashAddr+BYTES_NUM_EACH_TRAN;			//地址加256
				BL_Data.Totle_Frame--;										//剩余数据条数--
				ClearUsartBuf();
				
				if(BL_Data.Totle_Frame)//数据接收未完成
				{
					tem[0]=BL_Data.DeviceAddress;
					tem[1]=0xA7;
					tem[2]=(count&0xff00)>>8;
					tem[3]=	count&0x00ff;
					tem[4]=0x88;
					tem[5]=0xaa;
					M_crc=CRC16_Check(tem,6);
					tem[6] = (M_crc & 0xff);//校验低8位
					tem[7] = (M_crc >> 8);//校验高8位
					U485SendData(tem,8);//向485发送数据
					
					//log_info("SysTick_Count:%d\r\n",SysTick_Count);
				}
				else	//数据接收完成
				{
					tem[0]=BL_Data.DeviceAddress;
					tem[1]=0xB7;
					tem[2]=	 (BL_Data.Program_Size&0xff000000)>>24;
					tem[3]=	 (BL_Data.Program_Size&0x00ff0000)>>16;
					tem[4]=	 (BL_Data.Program_Size&0x0000ff00)>>8;
					tem[5]=	  BL_Data.Program_Size&0x000000ff;
					
					M_crc=CRC16_Check(tem,6);
					tem[6] = (M_crc & 0xff);//校验低8位
					tem[7] = (M_crc >> 8);//校验高8位
					U485SendData(tem,8);//向485发送数据
					
					//log_info("SysTick_Count:%d\r\n",SysTick_Count);
					
					tem[0]=0x08;tem[1]=0x08;tem[2]=0x08;tem[3]=0x08;
					EEWrite(Update_Firmware_Flag_Addr,tem,4);				//将固件更新完成标志位改为0x08080808
					if(((*(volatile unsigned int*)(FLASH_APP_ADDRESS + 4)) & 0xFF000000) == 0x08000000)//APP程序入口位置正确，执行跳转
					{
						log_info("Complete Firmware Update!Go To APP...\r\n");
						iap_load_app(FLASH_APP_ADDRESS);  //跳转至APP程序段
					}
					else 
					{
						log_info("Start APP Failed!\r\n");//如果跳转失败，则执行跳转指令（跳转至协议处）
						SystemResetSoft();
						while(1);
					}
				}
			}
			else 
			{
				ClearUsartBuf();
				//ModbusReturnAckInfo(02)//向总线发送错误
			}
		}
		ClearUsartBuf();
	}
	//return 0;
}

















