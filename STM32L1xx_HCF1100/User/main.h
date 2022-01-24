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
#include "ADXL355.h"
#include "HMC5883.h"
#include "IFlash.h"
#include "Modbus.h"



//����log_info
#define debug
#ifdef debug
		//#define log_info(...)    printf(__VA_ARGS__);
		 #define log_info(...)     GPIO_SetBits(GPIOA,GPIO_Pin_4);printf(__VA_ARGS__);GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#else
		#define log_info(...)
#endif

#define SystemResetSoft()								*((uint32_t *)0xe000ed0c)=0x05fa0004; //ʵ��ϵͳ�����λ

#define PI 3.1415926

#define KREEPROM_BASEADDR			  	0x32	 //���ּĴ����洢����ַ
#define FPOWERONFLAG_BASEADDR		  	0x00	   //�״ο�����־λ

#define ERROR			              	0		   //ʧ��
#define SUCCESS			            	1		   //�ɹ�

#define DEVICETYPE					  	0x044C	   //HCF1100�豸����,��ʮ����1100ת��Ϊʮ������
#define SOFTWAREVERSION			  	  	0x0402     //����汾�Ŷ�����򣺰汾�Ź���λ��ʾ������1.2.3����ʾ����Ϊ���߰�λ��ʾ1���Ͱ�λ��ʾ23,���еͰ�λ����ʾ99

#define DefaultDeviceADDR			  	0x01	   
#define DefaultDeviceMode	      		0x01	   
#define DefaultAIA      			  	1000
#define DefaultOriginalXACC  			0.0
#define DefaultOriginalYACC  			0.0
#define DefaultOriginalZACC  			0.0

#define DefaultOriginalXMAG  			0.0
#define DefaultOriginalYMAG  			0.0
#define DefaultOriginalZMAG  			0.0
#define DefaultDeviceLen				5         //�豸���ȣ���λ�ף�/10ʹ��

#define KeepRegister_Num	    (sizeof(KeepRegister)/2)
#define KeepRegister_Byte_Num    sizeof(KeepRegister)


#define LED1_ON()        GPIOB->BSRRH = GPIO_Pin_5//GPIO_ResetBits(GPIOB,GPIO_Pin_5)
#define LED1_OFF()    	 GPIOB->BSRRL = GPIO_Pin_5//GPIO_SetBits(GPIOB,GPIO_Pin_5)

#define RS485_RX()    	 GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define RS485_TX()    	 GPIO_SetBits(GPIOA,GPIO_Pin_4)

#define SenPower_ON()    GPIO_SetBits(GPIOB,GPIO_Pin_1)
#define SenPower_OFF()   GPIO_ResetBits(GPIOB,GPIO_Pin_1)





typedef struct __attribute__ ((__packed__))
{
	float	   XACC;								//ACC X  ���ٶ�ֵ
	float	   YACC;								//ACC Y
	float	   ZACC;								//ACC Z
	float	   TACC;								//ACC T
	float    ang_x;
	float    ang_y;
	float    ang_z;
	float	   XMAG;								//MAG Z��
	float	   YMAG;								//MAG Z��
	float	   ZMAG;								//MAG Z��
	float  	 azimuth;
	float    pitch;
	float    roll;
}IMU_Data_type;




/* Exported constants --------------------------------------------------------*/

extern volatile IMU_Data_type IMU_Data;
extern volatile unsigned int   WorkTime;
extern volatile unsigned char  WorkTimeOutFlag;
extern volatile unsigned char  Temp_Flag;            
extern volatile unsigned int   Temp_Count;	

extern volatile unsigned int SysTick_Count;
extern volatile unsigned int SysTick_Count1;
extern volatile unsigned int TimingDelay;


/* Exported functions ------------------------------------------------------- */
void Delay(volatile unsigned int nTime);

void Init_SysTick(void);
void Init_Devices(void);
void Init_Parameter(void);


void ClC_WatchDogTask(void);



extern float MKx,MKy,MKz;

extern unsigned char MAG_Flag;
















#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/







