#include "main.h"

/*****************************************************
* 【解锁Flash】     向Flash秘钥寄存器FLASH_KEYR中写入KEY1 = 0x45670123
* 						      再向Flash秘钥寄存器FLASH_KEYR中写入KEY2 = 0xCDEF89AB
* 【数据操作位数】
* 【擦除扇区】
* 【写入数据】
*****************************************************/

/****************************************************
* 内部Flash主存储器结构
* 内部Flash memory从0x0800 0000-0x0801 ffff，共计128KB
*	其中从0x0800 0000-0x0800 7fff共32KB用于存放Bootloader（hex）
*	其中从0x0x0800 8000-0x0801 ffff共计96KB用于存放应用程序(bin)
* 共计32个4KB扇区，每个扇区分为16个256B Page
* S0      PO          256B
*					P1					256B
*					P2          256B
*					P3					256B
*					P4-7        1KB
*					P8-11       1KB
*					P12-15      1KB
* S1									4KB
* S2									4KB
* ...
* S31									4KB
****************************************************/

volatile FLASH_Status FLASHStatus;

/*******************************************************************************
* Function Name  : Read_From_Flash
* Description    : 从Flash中读出数据
* Input          : Flash地址、数据输出指针、长度（8位,单字节）
* Output         : 返回数据
* Return         : None
*******************************************************************************/
void Read_NByte_From_Flash(unsigned int Addr,unsigned char *pBuffer,unsigned int Len)   	
{
	unsigned int i;
	for(i=0;i<Len;i++)
	{
		pBuffer[i]=*(volatile unsigned char*)Addr;//读取1个字节.
		Addr++;//偏移1个字节.	
	}
}
/*******************************************************************************
* Function Name  : Read_2NByte_From_Flash
* Description    : 从Flash中读出数据
* Input          : Flash地址、数据输出指针、长度（16位半字，双字节长度）
* Output         : 返回数据
* Return         : None
*******************************************************************************/
void Read_2NByte_From_Flash(unsigned int Addr,unsigned short *pBuffer,unsigned short Len)   	
{
	unsigned short i;
	for(i=0;i<Len;i++)
	{
		pBuffer[i]=*(volatile unsigned short*)Addr;//读取2个字节.
		Addr+=2;//偏移2个字节.	
	}
}

/*******************************************************************************
* Function Name  : Read_4NByte_From_Flash
* Description    : 从Flash中读出数据
* Input          : Flash地址、数据输出指针、长度（32位字，四字节长度）
* Output         : 返回数据
* Return         : None
*******************************************************************************/
void Read_4NByte_From_Flash(unsigned int Addr,unsigned int *pBuffer,unsigned int Len)   	
{
	unsigned int i;
	for(i=0;i<Len;i++)
	{
		pBuffer[i]=*(volatile unsigned int*)Addr;//读取2个字节.
		Addr+=4;//偏移2个字节.	
	}
}


/*******************************************************************************
* Function Name  : FLASH_If_Init
* Description    : 内部Flash初始化，解锁、清标志位
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_If_Init(void)
{ 
  /* Unlock the Program memory */
  FLASH_Unlock();

  /* Clear all FLASH flags */  
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);   
}
/*******************************************************************************
* Function Name  : FLASH_If_Erase
* Description    : 内部Flash擦除，从开始扇区，擦除整个Flash
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned int FLASH_If_Erase(uint32_t StartAddr)
{
  uint32_t flashaddress;
	
  flashaddress = StartAddr;
  SystemDisableIRQ();
  while (flashaddress <= (uint32_t) FLASH_LAST_PAGE_ADDRESS)
  {
    if (FLASH_ErasePage(flashaddress) == FLASH_COMPLETE)
    {
      flashaddress += FLASH_PAGE_SIZE;
    }
    else
    {
      /* Error occurred while page erase */
			SystemEnableIRQ();
      return (1);
    }
  }
	SystemEnableIRQ();
  return (0);
}
/*******************************************************************************
* Function Name  : FLASH_Page_Erase
* Description    : 内部Flash页擦除
* Input          : 扇区(0-31,其中APP占用8-31)、页(0-15)
* Output         : None
* Return         : None
*******************************************************************************/
unsigned int FLASH_Page_Erase(uint16_t sector,uint16_t Page)
{
  uint32_t flashaddress;
	if(sector<8 || sector>31 || Page>15 )return 1;
	
  flashaddress = FLASH_START_ADDRESS+sector*FLASH_SECTOR_SIZE+Page*FLASH_PAGE_SIZE;
	SystemDisableIRQ();
	FLASH_If_Init();
  FLASH_ErasePage(flashaddress);
	FLASH_Lock();
	SystemEnableIRQ();
  
  return (0);
}

/*******************************************************************************
* Function Name  : FLASH_Write
* Description    : 内部写Flash，从4N地址开始写
* Input          : 地址、数据指针、数据长度
* Output         : None
* Return         : None
*******************************************************************************/
unsigned int Write_4NByte_to_Flash(unsigned int FlashAddress, unsigned int* Pr ,unsigned short DataLength)
{
  //FLASH_Status status = FLASH_BUSY;
	unsigned int i;
	unsigned char status=0;
//	unsigned int sector_addr;    	//扇区地址
//	unsigned short sector_off;		//扇区偏移
//	unsigned short sector_cre;		//扇区内剩余地址
//	unsigned int   offaddr;				//去掉0x0800 0000后的地址
	
	if((FlashAddress<FLASH_APP_ADDRESS) || (FlashAddress>=FLASH_END_ADDRESS) )return 2;//地址合法检查
	if(FlashAddress % 4)return 2;//地址合法化检查，写起始地址必须是4的整数倍
	
//	offaddr=FlashAddress-FLASH_START_ADDRESS;		//相对于0x0800 0000的偏移量.
//	sector_addr=offaddr/FLASH_SECTOR_SIZE;    //计算扇区地址
//	
//	sector_off=(offaddr%FLASH_SECTOR_SIZE)/4;	//在扇区内的偏移(2个字节为基本单位.)
//	sector_cre=FLASH_SECTOR_SIZE/2-sector_off;//扇区剩余空间大小 
	SystemDisableIRQ();
	FLASH_If_Init();
  /* Write the buffer to the memory */
	for(i=0;i<DataLength;i++)
	{
    if( FLASH_FastProgramWord(FlashAddress, Pr[i]) == FLASH_COMPLETE)
		{
			FlashAddress=FlashAddress+4;
		}
		else 
		{
			status=1;
			break;
		}
	}
	FLASH_Lock();
	SystemEnableIRQ();
  return status;
}


void FLASH_Test(void)
{
		//Flash读写测试
	unsigned int buf[512]={0};
	unsigned int buf1[512]={0};
	unsigned int i=0;
	unsigned int test_ADDR=0;
	unsigned int count=0;
	
	
	count=64;
	for(i=0;i<count;i++)
	buf1[i]=i;
	test_ADDR=0x0801Ef00;
	FLASH_Page_Erase(30,15);
	FLASH_Page_Erase(31,0);
	Read_4NByte_From_Flash(test_ADDR,buf,count);  //OK 
	for(i=0;i<count;i++)
	log_info("Flash:%d\r\n",buf[i]);
	Write_4NByte_to_Flash(test_ADDR,buf1,count);
	Read_4NByte_From_Flash(test_ADDR,buf,count);  //OK 
	for(i=0;i<count;i++)
	log_info("AFlash:%d\r\n",buf[i]);
	
}
















