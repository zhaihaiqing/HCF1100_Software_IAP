#ifndef __IFLASH_H__
#define __IFLASH_H__

#include "stm32l1xx.h"

//用户根据自己的需要设置

#define STM32L1XX_MD

#define ABS_RETURN(x,y)         (x < y) ? (y-x) : (x-y)





void Read_NByte_From_Flash(unsigned int Addr,unsigned char *pBuffer,unsigned int Len);			//按字节读出数据
void Read_2NByte_From_Flash(unsigned int Addr,unsigned short *pBuffer,unsigned short Len);	//按半字读出数据
void Read_4NByte_From_Flash(unsigned int Addr,unsigned int *pBuffer,unsigned int Len); 			//按字读出数据

void FLASH_If_Init(void);
unsigned int FLASH_If_Erase(unsigned int StartAddr);
unsigned int FLASH_Page_Erase(uint16_t sector,uint16_t Page);

unsigned int Write_4NByte_to_Flash(unsigned int FlashAddress, unsigned int* Pr ,unsigned short DataLength);

void FLASH_Test(void);

#endif

















