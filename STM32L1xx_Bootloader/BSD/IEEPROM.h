
#ifndef __IEEPROM_H
#define __IEEPROM_H






void Write_EEPROM(unsigned int addr,unsigned char data);
unsigned char Read_EEPROM(unsigned int addr);

void EERead(unsigned short addr, unsigned char *pbuff,unsigned short length);
void EEWrite(unsigned short addr, unsigned char *pbuff,unsigned short length);
void EEErase(unsigned short addr, unsigned short length);

#endif


