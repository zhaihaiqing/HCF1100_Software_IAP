/**
  ******************************************************************************
  * @file    main.c 
  * @author  源清慧虹-翟海青
  * @version V4.0.0
  * @date    10-9-2018
  * @brief   Main program body
  ******************************************************************************
  */

#include "main.h"

float MKx=0,MKy=0,MKz=0;										//定义磁力计椭圆修正系数

volatile unsigned int SysTick_Count = 0;   //Systick计数
volatile unsigned int SysTick_Count1 = 0;   //Systick计数
volatile unsigned int TimingDelay = 0;     //延时函数计数

volatile unsigned int   WorkTime = 0;			//工作时间计时，最大900S，区别输入寄存器中的系统工作时间
volatile unsigned char  WorkTimeOutFlag = 0;	//工作时间溢出标志位，超过900S未收到指令，置1该位，同时不再喂狗，等待复位
volatile unsigned char  Temp_Flag=1;            //采样标志位，5s采一次
volatile unsigned int   Temp_Count=0;			//采样计数器

volatile IMU_Data_type IMU_Data;

/*******************************************************************************
* Function Name  : Delay function
* Description    : 延时函数，空闲时进入sleep模式
* Input          : 延时时长（ms）
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(volatile unsigned int nTime)
{ 
	TimingDelay = nTime;
	while(TimingDelay != 0);//__WFI;//等待过程中进入sleep模式
}

/*******************************************************************************
* Function Name  : SysTick init function
* Description    : 设置定时长度及SysTick优先级
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Init_SysTick(void)
{
	SysTick_Config(SystemCoreClock / 1000); //设置定时长度，1ms
	NVIC_SetPriority(SysTick_IRQn, 0x0);    //SysTick中断优先级设置
}

/*******************************************************************************
* Function Name  : Init_Devices function
* Description    : 完成硬件设备、各功能模块初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Init_Devices(void)
{
	//完成基本硬件初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//中断优先级分组
	GPIO_Configuration();
	Init_SysTick();//开启SysTick定时器
	UART1_Configuration(9600);
}

/*******************************************************************************
* Function Name  : Init_Parameter function
* Description    : 完成相应的参数初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Init_Parameter(void)
{
	uint8_t temp[2];
	//初始化保持寄存器参数
	//EEErase(0,128);
	EERead(FPOWERONFLAG_BASEADDR,temp,2);
	if( (temp[0] != 0x12) && (temp[1] != 0x34) )
	{
		//是首次开机
		EEErase(0,128);
		//EERead(KREEPROM_BASEADDR,(void *)&KeepRegister,sizeof(KeepRegister));//读取EEPROM中存储的保持寄存器值，恢复以下几项
		KeepRegister.DeviceAddress   = DefaultDeviceADDR;
		KeepRegister.DeviceMode      = DefaultDeviceMode;
		KeepRegister.IS_USER_MAG     = 0;
		KeepRegister.OriginalPitch   = 0;
		KeepRegister.OriginalRoll    = 0;
		KeepRegister.OriginalAzimuth = 0;
		
		KeepRegister.MAG_Mx    =1;
		KeepRegister.MAG_My    =1;
		KeepRegister.MAG_Mz    =1;
		
		KeepRegister.MAG_X_OFFSET    =0;
		KeepRegister.MAG_Y_OFFSET    =0;
		KeepRegister.MAG_Z_OFFSET    =0;	
		KeepRegister.DeviceLen       =DefaultDeviceLen;
		EEWrite(KREEPROM_BASEADDR,(void *)&KeepRegister,sizeof(KeepRegister));//更新EEPROM
		
		temp[0] = 0x12;
		temp[1] = 0x34;
		EEWrite(FPOWERONFLAG_BASEADDR,temp,2);
	}
	EERead(KREEPROM_BASEADDR,(void *)&KeepRegister,sizeof(KeepRegister));
	//参数非法检查，非法就更新为默认值
	
	//
	if( (KeepRegister.DeviceAddress<1) || (KeepRegister.DeviceAddress>247) )KeepRegister.DeviceAddress=DefaultDeviceADDR;	//检查地址范围是否在1-247，否则恢复默认
	if( (KeepRegister.DeviceMode<1) || (KeepRegister.DeviceMode>4) )KeepRegister.DeviceMode=DefaultDeviceMode;						//检查模式范围是否在1-4，否则恢复默认
	//if( (KeepRegister.IS_USER_MAG<1) || (KeepRegister.IS_USER_MAG>4) )KeepRegister.IS_USER_MAG=DefaultAIA;																				//检查平均数量范围是否在1-1000，否则恢复默认
	if( (KeepRegister.DeviceLen<1) || (KeepRegister.DeviceLen>10) )KeepRegister.DeviceLen=DefaultDeviceLen;								//检查设备长度范围是否在0.1-1米，否则恢复默认
	
	InputRegister.DeviceType      = DEVICETYPE;
	InputRegister.SoftwareVersion = SOFTWAREVERSION;
	InputRegister.Len = KeepRegister.DeviceLen;
	Get_SNInfo_Fun();
	
}

/*******************************************************************************
* Function Name  : ClC_WatchDogTask
* Description    : 看门狗任务
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ClC_WatchDogTask(void)
{
//	if(!WorkTimeOutFlag) 
//	{
//		//GPIOA->ODR ^= GPIO_Pin_3;//喂狗操作；如果超过913秒内未收到任何指令，执行系统重启，否则喂狗。
//		GPIO_PinReverse(GPIOA,GPIO_Pin_3);
//	}   
	GPIO_PinReverse(GPIOA,GPIO_Pin_3);
}
/*******************************************************************************
* Function Name  : main
* Description    : 主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
	SCB->VTOR = FLASH_BASE | 0x8000;      //设置向量表偏移值，Bootloader起始地址为0x0800 0000，共32KB空间,APP程序起始地址为0x0800 8000，
	
	Init_Devices();												//初始化设备
	Init_Parameter();											//初始化参数
	ClC_WatchDogTask();										//看门狗任务
	Delay(100);														//启动延时
	HMC5883L_Init();											//初始化磁力计，包含I2C初始化
	ClC_WatchDogTask();
	ADXL355_Init();												//初始化加速度计，包含SPI初始化
	ClC_WatchDogTask();										//看门狗任务
	RS485_RX();														//RS485切换为接收模式
	Delay(10);
	
	//log_info("addr:%d\r\n",KeepRegister.DeviceAddress);
  while (1)
  {
		ProcessTask();											//进入任务函数
		ClC_WatchDogTask();		
		
  }
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
