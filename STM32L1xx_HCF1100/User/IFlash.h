
#ifndef __IFLASH_H
#define __IFLASH_H

#define EEPROM_START_ADDR 	0x08080280
#define EEPROM_SIZE       	0x800
#define EEPROM_END_ADDR 	0x080807FF

//定义BL与APP共享空间起始地址
#define	EEPROM_SHARE_DATA_ADDR	0x180
#define	EEPROM_SHARE_DATA_SPACE	0x80

#define PEKEY1        0x89ABCDEF                //FLASH_PEKEYR
#define PEKEY2        0x02030405                //FLASH_PEKEYR    以下分别实现按字节和字方式写入：

void Write_EEPROM(unsigned int addr,unsigned char data);
unsigned char Read_EEPROM(unsigned int addr);


void EERead(unsigned short addr, unsigned char *pbuff,unsigned short length);
void EERead_Z(unsigned short addr, unsigned char *pbuff,unsigned short length);
void EEWrite(unsigned short addr, unsigned char *pbuff,unsigned short length);
void EEErase(unsigned short addr, unsigned short length);

#endif


