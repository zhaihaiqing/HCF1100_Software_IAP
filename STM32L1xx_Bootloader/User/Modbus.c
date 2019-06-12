
#include "main.h"

volatile unsigned char Modbus_BusyFlag;				//����Modbusæ��־λ
volatile unsigned char Modbus_FreeFlag;				//����Modbus���б�־λ
volatile unsigned char ModbusIntervalTime = Default_USART1_INTERVAL_TIME;//ָ��ͼ��ʱ��,��ʱ��Ϊһ��ָ���ѽ������

volatile ModbusDataPackage_type 	ModbusDataPackage;	//����modbus���ջ���

/*******************************************************************************
* Function Name  : __ltobf
* Description    : float�����ݴ�С�˸�ʽת��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//static float __ltobf(float data)
//{
//	FLOAT_CONV d1, d2;

//	d1.f = data;

//	d2.c[0] = d1.c[3];
//	d2.c[1] = d1.c[2];
//	d2.c[2] = d1.c[1];
//	d2.c[3] = d1.c[0];
//	return d2.f;
//}
/*******************************************************************************
* Function Name  : CRC16_Check
* Description    : CRCУ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t CRC16_Check(uint8_t *Pushdata,uint8_t length)
{
	uint16_t Reg_CRC=0xffff;
	uint8_t i,j;
	for( i = 0; i<length; i ++)
	{
		Reg_CRC^= *Pushdata++;
		for (j = 0; j<8; j++)
		{
			if (Reg_CRC & 0x0001)

			Reg_CRC=Reg_CRC>>1^0xA001;
			else
			Reg_CRC >>=1;
		}
	}
	return   Reg_CRC;
}

/*******************************************************************************
* Function Name  : ModbusReturnAckInfo
* Description    : modbus�����쳣����Ϣ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void ModbusReturnAckInfo(uint8_t err)
//{
//	uint8_t ErrDat[5];
//	uint16_t crc;
//	//log_info("err:0x%x\r\n",err);
//	if(err)//�쳣��
//	{
//		ErrDat[0] = BL_Data.DeviceAddress;//��ֵ�豸��ַ
//		ErrDat[1] = ModbusDataPackage.dat[1];//��ֵ�쳣������,��������+0x80
//		ErrDat[2] = err;//��ֵ�쳣��
//		crc = CRC16_Check(ErrDat,3);
//		ErrDat[3] = (crc & 0xff);//У���8λ
//		ErrDat[4] = (crc >> 8);//У���8λ
//		
//		U485SendData(ErrDat,5);//��485��������
//		//err=0;
//	}
//}

/***********************************************************************
��ȡSN��Ϣ
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x40)+������(4�ֽ�)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x40)+��������(1�ֽ�)+SN��Ϣ(15�ֽ�)+У��(2�ֽ�)
������:0x33,0x55,0x77,0x99
************************************************************************/
const char __progmem_smartbow_start[32] = "**SUP43214321123**4";
char Get_SNInfo_Fun(void)
{
	unsigned char i;
	unsigned char temp[19]={0};
	unsigned char SN_Program[16]={0};
	unsigned char SN_EEPROM[16]={0};
	unsigned char SN_ERR_FLAG=0;
	
	for(i=0; i<19; i++)
	{
		temp[i] = __progmem_smartbow_start[i];
	}
	
	for(i=0; i<14; i++)
	{
		SN_Program[i] = temp[i+2];
	}
	SN_Program[14]=temp[18];
	SN_Program[15]=0;
	
	for(i=0;i<8;i++)
	{
		BL_Data.SN[i]=SN_Program[i*2]<<8 | SN_Program[i*2+1];
	}
	
	EERead(EEPROM_SHARE_DATA_ADDR,SN_EEPROM,16);//��ȡEEPROM�й����ֶ��е�SN��Ϣ
	
	for(i=0;i<16;i++)
	{
		log_info("SN_Program[%d]:0x%x----SN_EEPROM[%d]:0x%x\r\n",i,SN_Program[i],i,SN_EEPROM[i]);
	}
	
	//ѭ���ж�ROM�е�SN��Program�е�SN�Ƿ���ȣ��������������EEPROMROM��д��SN��Ϣ
	for(i=0;i<16;i++)
	{
		if(SN_EEPROM[i] != SN_Program[i]) SN_ERR_FLAG++;
	}
	if(SN_ERR_FLAG)
	{
		EEWrite(EEPROM_SHARE_DATA_ADDR,SN_Program,16);
	}
	log_info("SN Init OK!SN_ERR_FLAG:%d\r\n",SN_ERR_FLAG);
	
	return SUCCESS;
}














