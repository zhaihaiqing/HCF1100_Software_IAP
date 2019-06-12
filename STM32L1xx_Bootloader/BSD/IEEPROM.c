/* Includes ------------------------------------------------------------------*/
#include "main.h"

/*******************************************************************************
* Function Name  : Write_EEPROM
* Description    : 向内部EEPROM写入一个字节数据
* Input          : 地址，数据
* Output         : None
* Return         : None
*******************************************************************************/
void Write_EEPROM(unsigned int addr,unsigned char data)
{
	unsigned int address=0;
	address=EEPROM_START_ADDR+addr;
	
	if(IS_FLASH_DATA_ADDRESS(address))
	{
		DATA_EEPROM_Unlock();
		while(FLASH_GetStatus()!=FLASH_COMPLETE);
		DATA_EEPROM_ProgramByte(address,data);
		DATA_EEPROM_Lock();
	}
}

/*******************************************************************************
* Function Name  : Read_EEPROM
* Description    : 从内部EEPROM读出一个字节数据
* Input          : 地址
* Output         : None
* Return         : 数据
*******************************************************************************/
unsigned char Read_EEPROM(unsigned int addr)
{
	unsigned int address=0;
	unsigned char tmp=0;
	address=EEPROM_START_ADDR+addr;
	
	if(IS_FLASH_DATA_ADDRESS(address))
	{
		DATA_EEPROM_Unlock();
		while(FLASH_GetStatus()==FLASH_BUSY);
		tmp=*(__IO unsigned char *)address;
		DATA_EEPROM_Lock();
	}
	return tmp;
}

/*******************************************************************************
* Function Name  : EEWrite
* Description    : 向内部EEPROM写入一串数据
* Input          : 地址，数据
* Output         : None
* Return         : None
*******************************************************************************/
void EEWrite(unsigned short addr, unsigned char *pbuff,unsigned short length)
{
	unsigned char *waddr=NULL;
	waddr=(unsigned char *)(EEPROM_START_ADDR+addr);
	
	SystemDisableIRQ();
	DATA_EEPROM_Unlock();
	while(FLASH_GetStatus()!=FLASH_COMPLETE);
	while(length--)
	{
		*waddr++=*pbuff++;
		while(FLASH_GetStatus()==FLASH_BUSY);
	}
	DATA_EEPROM_Lock();
	SystemEnableIRQ();
}

/*******************************************************************************
* Function Name  : Read_EEPROM
* Description    : 从内部EEPROM读出一串数据
* Input          : 首地址，长度
* Output         : 数据
* Return         : None
*******************************************************************************/
void EERead(unsigned short addr, unsigned char *pbuff,unsigned short length)
{
	unsigned char *waddr=NULL;
	waddr=(unsigned char *)(EEPROM_START_ADDR+addr);
	
	SystemDisableIRQ();
	DATA_EEPROM_Unlock();
	while(FLASH_GetStatus()==FLASH_BUSY);
	while(length--)
	{
		*pbuff++=*waddr++;
	}
	DATA_EEPROM_Lock();
	SystemEnableIRQ();
}

/*******************************************************************************
* Function Name  : EEErase
* Description    : EEPROM擦除函数，写0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EEErase(unsigned short addr, unsigned short length)
{
	unsigned char Data_temp[256]={0x00};
	if(length>256)length=256;
	EEWrite(addr,(void *)&Data_temp,length);
}




















