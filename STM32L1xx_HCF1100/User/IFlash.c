/* Includes ------------------------------------------------------------------*/
#include "main.h"


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


void EERead(unsigned short addr, unsigned char *pbuff,unsigned short length)
{
	unsigned char *waddr=NULL;
	waddr=(unsigned char *)(EEPROM_START_ADDR+addr);
	
	DATA_EEPROM_Unlock();
	while(FLASH_GetStatus()==FLASH_BUSY);
	while(length--)
	{
		*pbuff++=*waddr++;
	}
	DATA_EEPROM_Lock();
}

void EERead_Z(unsigned short addr, unsigned char *pbuff,unsigned short length)
{
	unsigned char *waddr=NULL;
	waddr=(unsigned char *)(0x08080000+addr);
	
	DATA_EEPROM_Unlock();
	while(FLASH_GetStatus()==FLASH_BUSY);
	while(length--)
	{
		*pbuff++=*waddr++;
	}
	DATA_EEPROM_Lock();
}

void EEWrite(unsigned short addr, unsigned char *pbuff,unsigned short length)
{
	unsigned char *waddr=NULL;
	waddr=(unsigned char *)(EEPROM_START_ADDR+addr);
	
	DATA_EEPROM_Unlock();
	while(FLASH_GetStatus()!=FLASH_COMPLETE);
	while(length--)
	{
		*waddr++=*pbuff++;
		while(FLASH_GetStatus()==FLASH_BUSY);
	}
	DATA_EEPROM_Lock();
	
}

/*******************************************************************************
* Function Name  : EEErase
* Description    : EEPROM²Á³ýº¯Êý£¬Ð´0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EEErase(unsigned short addr, unsigned short length)
{
	unsigned char Data_temp[96]={0x00};
	if(length>96)length=96;
	EEWrite(addr,(void *)&Data_temp,length);
}




















