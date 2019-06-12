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

volatile unsigned int SysTick_Count = 0;   //Systick计数
volatile unsigned int TimingDelay = 0;     //延时函数计数
const unsigned char  FactorySetValueWord[4] = {0x44,0x66,0x88,0xaa};//出厂配置指令代号


unsigned int First_PO_Flag = 0;     				//首次上电标志位					存储位置：EEPROM_BL_FLAG_ADDR+0

//固件更新标志位，采用4位，值为0x08080808时执行更新，值为0x05050505时表示更新完成
unsigned int Update_Firmware_Flag = 0;     	//APP程序更新标志位		存储位置：EEPROM_BL_FLAG_ADDR+4


volatile BL_Data_type BL_Data;

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

void Init_Hardware(void)
{
	//完成硬件初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//中断优先级分组
	Init_SysTick();//开启SysTick定时器
	GPIO_Configuration();
	UART1_Configuration(9600);

#ifdef  debug	
	UART3_Configuration(115200);
#endif
	
	log_info("Run in BootLoader,Hardware has init!\r\n");
	
	RS485_RX();
	Delay(5);
}

void Init_Parameter(void)
{
	uint8_t	tem[12]={0};
	
	Delay(5);								//启动延时
	
	
	BL_Data.APP_FlashAddr = FLASH_APP_ADDRESS;										  //APP程序起始地址
	BL_Data.Start_sector  = (FLASH_APP_ADDRESS-FLASH_START_ADDRESS)/FLASH_SECTOR_SIZE;//计算APP程序起始扇区
	
	//读取BL标志位
	EERead(First_PO_Flag_Addr,tem,8);
	First_PO_Flag			=		tem[3]<<24 | tem[2]<<16 | tem[1]<<8 | tem[0];//首次开机标志位
	Update_Firmware_Flag	=		tem[7]<<24 | tem[6]<<16 | tem[5]<<8 | tem[4];//固件升级标志位
	
	//tem[0]=0x08;tem[1]=0x08;tem[2]=0x08;tem[3]=0x08; //写更新标志位
	//EEWrite(Update_Firmware_Flag_ADDR,tem,4)

	BL_Data.DeviceType=DEVICETYPE;
	BL_Data.SoftwareVersion=SOFTWAREVERSION;
	
	Get_SNInfo_Fun();		//获取SN信息，同时读取EEPROM中SN信息，判断一致性，不一致重新写EEPROM
	
	
	ClC_WatchDogTask();//等待MT消息
	
	
	//如果需要更新，读取和APP相关的数据
	EERead(EEPROM_APP_DATA_ADDR,tem,2);		//读取设备485通讯地址
	BL_Data.DeviceAddress= tem[0];
	if( (BL_Data.DeviceAddress<1) || (BL_Data.DeviceAddress>247) )	
	{
		BL_Data.DeviceAddress=1;
	}
	
	log_info("Parameter has init! Device_ADDR:%d\r\n",BL_Data.DeviceAddress);
	log_info("First_PO_Flag:0x%x \r\n",First_PO_Flag);
	log_info("Update_Firmware_Flag:0x%x \r\n",Update_Firmware_Flag);
	log_info("DeviceType:0x%x \r\n",BL_Data.DeviceType);
	log_info("SoftwareVersion:0x%x \r\n",BL_Data.SoftwareVersion);
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
	Init_Hardware();	//完成硬件驱动初始化
	Init_Parameter();	//完成参数初始化
	
	
	while(1)
	{
		//如果已升级完成标志位==0x08080808，直接跳转至APP程序
		if( Update_Firmware_Flag == 0x08080808 )
		{
			ClC_WatchDogTask();	//等待MT消息
			Wait_Commend();		//在跳转前5秒收到升级指令，执行Updata，防止程序启动失败
			
			if( (Update_Firmware_Flag == 0x08080808) && (((*(volatile unsigned int*)(FLASH_APP_ADDRESS + 4)) & 0xFF000000) == 0x08000000) )//APP程序入口位置正确，执行跳转
			{
				//执行IAP跳转
				log_info("No Update Task , Go To APP ....\r\n");
				ClC_WatchDogTask();	  
				iap_load_app(FLASH_APP_ADDRESS);  //跳转至APP程序段
			}
			else 
			{
				log_info("Start APP Failed!\r\n");//如果跳转失败，等待看门狗复位
				Update_Firmware();
				SystemResetSoft();//固件升级失败直接重启
				while(1);
			}
		}
		//否则执行升级
		else
		{
			log_info("Update_Firmware...\r\n");
			
			Update_Firmware();										//执行固件升级
			SystemResetSoft();//固件升级失败直接重启
			while(1);
		}
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
