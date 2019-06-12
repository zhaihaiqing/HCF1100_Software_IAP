
#ifndef __IAP_CONFIG_H
#define __IAP_CONFIG_H


#define	BYTES_NUM_EACH_TRAN			256			//定义每包数据中包含的镜像字节数，必须输4的整数倍
/***********          IO配置           *************************/

#define ClC_WatchDogTask()			GPIOA->ODR ^= GPIO_Pin_3	//看门狗喂狗

#define RS485_RX()    	 			GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define RS485_TX()    	 			GPIO_SetBits(GPIOA,GPIO_Pin_4)

//
//#define	USART1_Tx	GPIO_Pin_6
//#define	USART1_Rx	GPIO_Pin_7

//USART

/***********          Flash配置           *************************/
//STM32L151CBT6-A  Cat.2
//Flash:划分为0-31个扇区，每个扇区划分为0-15个page，每页256Bytes


#define STM32_FLASH_SIZE  128 	 					//所选STM32的FLASH容量大小(单位为KB)
#define FLASH_SECTOR_SIZE 4096 						//扇区大小（4096个字节），STM32L151CBT6-A  Cat.2
#define FLASH_PAGE_SIZE   256						//每页256个字节

#define FLASH_SECTOR_TOTAL_NUM			(STM32_FLASH_SIZE*1024/FLASH_SECTOR_SIZE)
#define FLASH_PAGE_NUM_EACH_SECTOR		(FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE)

//FLASH起始地址
#define FLASH_START_ADDRESS 0x08000000 	     		//STM32 FLASH的起始地址,BootLoader起始地址
#define BootLoader_MAXSIZE  0x8000   				//STM32 BOOTLoader程序最大长度32K

#define FLASH_APP_ADDRESS 	0x08008000 				//STM32 FLASH的起始地址  APP程序起始地址
#define APP_MAXSIZE   		0x18000									//STM32 APP程序最大长度96K

#define FLASH_LAST_PAGE_ADDRESS  		0x0801FF00	//Flash最后一页的起始地址
#define FLASH_END_ADDRESS        		0x0801FFFF	//Flash最后一个地址

#define FLASH_SECTOR_NUMBER  	  ((uint32_t)(ABS_RETURN(FLASH_APP_ADDRESS,FLASH_START_ADDRESS))>>12)

#define FLASH_PROTECTED_SECTORS   ((uint32_t)~((1 << FLASH_SECTOR_NUMBER) - 1))

#define FLASH_SIZE   			 (FLASH_END_ADDRESS - FLASH_APP_ADDRESS + 1)



/***********          IEEPROM配置           *************************/

#define KREEPROM_BASEADDR			  	0x32	 //保持寄存器存储基地址



#define PEKEY1        0x89ABCDEF                //FLASH_PEKEYR
#define PEKEY2        0x02030405                //FLASH_PEKEYR    以下分别实现按字节和字方式写入：

#define EEPROM_START_ADDR 	0x08080000        //内部EEPROM起始地址
#define EEPROM_SIZE       	0x0FFF			  //内部EEPROM空间大小 4KB
#define EEPROM_END_ADDR 	0x08080FFF		  //内部EEPROM结束地址

//定义BootLoader标志位起始地址及大小
#define	EEPROM_BL_FLAG_ADDR		0x00
#define	EEPROM_BL_FLAG_SPACE	0x80

//定义BootLoader数据起始地址及大小
#define	EEPROM_BL_DATA_ADDR		0x80
#define	EEPROM_BL_DATA_SPACE	0x100

//定义BL与APP共享空间起始地址
#define	EEPROM_SHARE_DATA_ADDR	0x180
#define	EEPROM_SHARE_DATA_SPACE	0x80

//定义APP标志位起始地址及大小
#define	EEPROM_APP_FLAG_ADDR	0x200
#define	EEPROM_APP_FLAG_SPACE	0x80

//定义APP数据起始地址及大小
#define	EEPROM_APP_DATA_ADDR	0x280
#define	EEPROM_APP_DATA_SPACE	0x800

#define First_PO_Flag_Addr 				 	EEPROM_BL_FLAG_ADDR+0     			//首次开机标志位存储位置
#define Update_Firmware_Flag_Addr 		 	EEPROM_BL_FLAG_ADDR+4   			//APP程序更新标志位存储位置







#endif


