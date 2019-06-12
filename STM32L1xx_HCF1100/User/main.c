/**
  ******************************************************************************
  * @file    main.c 
  * @author  Դ��ۺ�-�Ժ���
  * @version V4.0.0
  * @date    10-9-2018
  * @brief   Main program body
  ******************************************************************************
  */

#include "main.h"

float MKx=0,MKy=0,MKz=0;										//�����������Բ����ϵ��

volatile unsigned int SysTick_Count = 0;   //Systick����
volatile unsigned int SysTick_Count1 = 0;   //Systick����
volatile unsigned int TimingDelay = 0;     //��ʱ��������

volatile unsigned int   WorkTime = 0;			//����ʱ���ʱ�����900S����������Ĵ����е�ϵͳ����ʱ��
volatile unsigned char  WorkTimeOutFlag = 0;	//����ʱ�������־λ������900Sδ�յ�ָ���1��λ��ͬʱ����ι�����ȴ���λ
volatile unsigned char  Temp_Flag=1;            //������־λ��5s��һ��
volatile unsigned int   Temp_Count=0;			//����������

volatile IMU_Data_type IMU_Data;

/*******************************************************************************
* Function Name  : Delay function
* Description    : ��ʱ����������ʱ����sleepģʽ
* Input          : ��ʱʱ����ms��
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(volatile unsigned int nTime)
{ 
	TimingDelay = nTime;
	while(TimingDelay != 0);//__WFI;//�ȴ������н���sleepģʽ
}

/*******************************************************************************
* Function Name  : SysTick init function
* Description    : ���ö�ʱ���ȼ�SysTick���ȼ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Init_SysTick(void)
{
	SysTick_Config(SystemCoreClock / 1000); //���ö�ʱ���ȣ�1ms
	NVIC_SetPriority(SysTick_IRQn, 0x0);    //SysTick�ж����ȼ�����
}

/*******************************************************************************
* Function Name  : Init_Devices function
* Description    : ���Ӳ���豸��������ģ���ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Init_Devices(void)
{
	//��ɻ���Ӳ����ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//�ж����ȼ�����
	GPIO_Configuration();
	Init_SysTick();//����SysTick��ʱ��
	UART1_Configuration(9600);
}

/*******************************************************************************
* Function Name  : Init_Parameter function
* Description    : �����Ӧ�Ĳ�����ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Init_Parameter(void)
{
	uint8_t temp[2];
	//��ʼ�����ּĴ�������
	//EEErase(0,128);
	EERead(FPOWERONFLAG_BASEADDR,temp,2);
	if( (temp[0] != 0x12) && (temp[1] != 0x34) )
	{
		//���״ο���
		EEErase(0,128);
		//EERead(KREEPROM_BASEADDR,(void *)&KeepRegister,sizeof(KeepRegister));//��ȡEEPROM�д洢�ı��ּĴ���ֵ���ָ����¼���
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
		EEWrite(KREEPROM_BASEADDR,(void *)&KeepRegister,sizeof(KeepRegister));//����EEPROM
		
		temp[0] = 0x12;
		temp[1] = 0x34;
		EEWrite(FPOWERONFLAG_BASEADDR,temp,2);
	}
	EERead(KREEPROM_BASEADDR,(void *)&KeepRegister,sizeof(KeepRegister));
	//�����Ƿ���飬�Ƿ��͸���ΪĬ��ֵ
	
	//
	if( (KeepRegister.DeviceAddress<1) || (KeepRegister.DeviceAddress>247) )KeepRegister.DeviceAddress=DefaultDeviceADDR;	//����ַ��Χ�Ƿ���1-247������ָ�Ĭ��
	if( (KeepRegister.DeviceMode<1) || (KeepRegister.DeviceMode>4) )KeepRegister.DeviceMode=DefaultDeviceMode;						//���ģʽ��Χ�Ƿ���1-4������ָ�Ĭ��
	//if( (KeepRegister.IS_USER_MAG<1) || (KeepRegister.IS_USER_MAG>4) )KeepRegister.IS_USER_MAG=DefaultAIA;																				//���ƽ��������Χ�Ƿ���1-1000������ָ�Ĭ��
	if( (KeepRegister.DeviceLen<1) || (KeepRegister.DeviceLen>10) )KeepRegister.DeviceLen=DefaultDeviceLen;								//����豸���ȷ�Χ�Ƿ���0.1-1�ף�����ָ�Ĭ��
	
	InputRegister.DeviceType      = DEVICETYPE;
	InputRegister.SoftwareVersion = SOFTWAREVERSION;
	InputRegister.Len = KeepRegister.DeviceLen;
	Get_SNInfo_Fun();
	
}

/*******************************************************************************
* Function Name  : ClC_WatchDogTask
* Description    : ���Ź�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ClC_WatchDogTask(void)
{
//	if(!WorkTimeOutFlag) 
//	{
//		//GPIOA->ODR ^= GPIO_Pin_3;//ι���������������913����δ�յ��κ�ָ�ִ��ϵͳ����������ι����
//		GPIO_PinReverse(GPIOA,GPIO_Pin_3);
//	}   
	GPIO_PinReverse(GPIOA,GPIO_Pin_3);
}
/*******************************************************************************
* Function Name  : main
* Description    : ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
	SCB->VTOR = FLASH_BASE | 0x8000;      //����������ƫ��ֵ��Bootloader��ʼ��ַΪ0x0800 0000����32KB�ռ�,APP������ʼ��ַΪ0x0800 8000��
	
	Init_Devices();												//��ʼ���豸
	Init_Parameter();											//��ʼ������
	ClC_WatchDogTask();										//���Ź�����
	Delay(100);														//������ʱ
	HMC5883L_Init();											//��ʼ�������ƣ�����I2C��ʼ��
	ClC_WatchDogTask();
	ADXL355_Init();												//��ʼ�����ٶȼƣ�����SPI��ʼ��
	ClC_WatchDogTask();										//���Ź�����
	RS485_RX();														//RS485�л�Ϊ����ģʽ
	Delay(10);
	
	//log_info("addr:%d\r\n",KeepRegister.DeviceAddress);
  while (1)
  {
		ProcessTask();											//����������
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
