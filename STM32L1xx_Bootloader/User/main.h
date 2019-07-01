/**
  ******************************************************************************
  * @file    Project/STM32L1xx_StdPeriph_Templates/main.h 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    16-May-2014
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "stm32l1xx.h"
#include "uart.h"
#include "gpio.h"
#include "Modbus.h"

#include "IEEPROM.h"
#include "IFLASH.h"
#include "IAP.h"
#include "IAP_CONFIG.h"


#pragma pack(2)  //指定两字节对齐


//定义log_info
//#define debug
#ifdef  debug
		//#define log_info(...)             printf(__VA_ARGS__);
		 #define log_info(...)          GPIO_SetBits(GPIOA,GPIO_Pin_4);printf(__VA_ARGS__);GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#else
		#define log_info(...)
#endif


#define SystemResetSoft()							*((uint32_t *)0xe000ed0c)=0x05fa0004; //实现系统软件复位
#define SystemDisableIRQ()							__disable_irq()//关闭总中断{ __ASM volatile ("cpsid i"); }
#define SystemEnableIRQ()							__enable_irq() //开启总中断{ __ASM volatile ("cpsie i"); }




#define ERROR			              	    0		   //失败
#define SUCCESS			            	    1		   //成功

#define DEVICETYPE					        0x044C	   //HCF1100设备类型,将十进制1100转换为十六进制
#define SOFTWAREVERSION			        	0x0402     //软件版本号定义规则：版本号共三位表示，例如1.2.3，表示方法为：高八位表示1，低八位表示23,其中低八位最大表示99

#define DefaultDeviceADDR			        0x01	   
	   

#define KeepRegister_Num	               (sizeof(KeepRegister)/2)
#define KeepRegister_Byte_Num         		sizeof(KeepRegister)


#define LED1_ON()        GPIOB->BSRRH = GPIO_Pin_5//GPIO_ResetBits(GPIOB,GPIO_Pin_5)
#define LED1_OFF()    	 GPIOB->BSRRL = GPIO_Pin_5//GPIO_SetBits(GPIOB,GPIO_Pin_5)



//BL数据结构体
typedef struct __attribute__ ((__packed__))
{
	unsigned short 	 DeviceAddress;				//设备的485地址
	unsigned short 	 DeviceType;				//设备类型
	unsigned short   SoftwareVersion;			//BL软件版本
	
	unsigned short   SN[8];
	
	unsigned short 	Program_Version;	//APP程序版本
	unsigned int   	Program_Size;		//APP程序字节数
	unsigned short 	Totle_Frame;		//定义数据总条数
	
	unsigned short 	Totle_Frame_Count;	//定义已接收数据总条数计数器
	unsigned char 	Start_sector;		//起始位置为0x08008000,位于第8扇区
	unsigned char 	End_sector;			//结束扇区
	unsigned char 	Start_Page;			//起始页
	unsigned char 	End_Page;			//结束页
	unsigned int 	APP_FlashAddr;		//
	
}BL_Data_type;





/* Exported constants --------------------------------------------------------*/
extern volatile unsigned int SysTick_Count;
extern volatile unsigned int TimingDelay;

extern volatile BL_Data_type BL_Data;

/* Exported functions ------------------------------------------------------- */
void Delay(volatile unsigned int nTime);

void Init_SysTick(void);
void Init_Devices(void);
void Init_Parameter(void);


//extern unsigned short Device_ADDR;
extern const unsigned char  FactorySetValueWord[4];
extern unsigned int Update_Firmware_Flag;














#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/







