
#include "main.h"

volatile unsigned char Modbus_BusyFlag;				//定义Modbus忙标志位
volatile unsigned char Modbus_FreeFlag;				//定义Modbus空闲标志位
volatile unsigned char ModbusIntervalTime = Default_USART1_INTERVAL_TIME;//指令发送间隔时间,超时认为一条指令已接收完毕

volatile ModbusDataPackage_type 	ModbusDataPackage;	//定义modbus接收缓存

/*******************************************************************************
* Function Name  : __ltobf
* Description    : float型数据大小端格式转换
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
* Description    : CRC校验
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
* Description    : modbus返回异常码信息
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void ModbusReturnAckInfo(uint8_t err)
//{
//	uint8_t ErrDat[5];
//	uint16_t crc;
//	//log_info("err:0x%x\r\n",err);
//	if(err)//异常码
//	{
//		ErrDat[0] = BL_Data.DeviceAddress;//赋值设备地址
//		ErrDat[1] = ModbusDataPackage.dat[1];//赋值异常功能码,即功能码+0x80
//		ErrDat[2] = err;//赋值异常码
//		crc = CRC16_Check(ErrDat,3);
//		ErrDat[3] = (crc & 0xff);//校验低8位
//		ErrDat[4] = (crc >> 8);//校验高8位
//		
//		U485SendData(ErrDat,5);//向485发送数据
//		//err=0;
//	}
//}

/***********************************************************************
获取SN信息
数据格式:器件地址(1字节)+功能码(0x40)+操作码(4字节)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x40)+数据数量(1字节)+SN信息(15字节)+校验(2字节)
操作码:0x33,0x55,0x77,0x99
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
	
	EERead(EEPROM_SHARE_DATA_ADDR,SN_EEPROM,16);//读取EEPROM中共享字段中的SN信息
	
	for(i=0;i<16;i++)
	{
		log_info("SN_Program[%d]:0x%x----SN_EEPROM[%d]:0x%x\r\n",i,SN_Program[i],i,SN_EEPROM[i]);
	}
	
	//循环判断ROM中的SN和Program中的SN是否相等，不相等则重新向EEPROMROM中写入SN信息
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














