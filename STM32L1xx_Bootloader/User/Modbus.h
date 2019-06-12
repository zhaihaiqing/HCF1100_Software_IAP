#ifndef __MODBUS_H
#define __MODBUS_H

////大小端转换
//#define htons(n) ((((n) & 0x00ff) << 8) | (((n) & 0xff00) >> 8))
//#define ntohs(n) htons(n)
//#define htonl(n) ((((n) & 0x000000ff) << 24)|  \
//				 (((n) & 0x0000ff00) << 8)  |  \
//				 (((n) & 0x00ff0000) >> 8)  |  \
//				 (((n) & 0xff000000) >> 24))
//#define ntohl(n) htonl(n)
//#define htond(n) (htonl(n & 0xffffffff) << 32) | htonl(n >> 32)
//#define ntohd(n) htond(n)

//typedef union{													 //float型数据大小端格式转换
//	float f;
//	char c[4];
//}FLOAT_CONV;


//modbus 消息接收结构体
typedef struct __attribute__ ((__packed__))
{
	unsigned char DataFlag;					//数据标记,是否有数据
	unsigned short DataLen;					//数据长度
	unsigned char dat[UART1_RBUF_SIZE];	    //数据缓存
}ModbusDataPackage_type;


extern volatile unsigned char ModbusIntervalTime;
extern volatile ModbusDataPackage_type ModbusDataPackage;	//定义modbus接收缓存
extern volatile unsigned char Modbus_BusyFlag;
extern volatile unsigned char Modbus_FreeFlag;

static float __ltobf(float data);
unsigned short CRC16_Check(unsigned char *Pushdata,unsigned char length);
void ModbusReturnAckInfo(unsigned char err);

void InstructionTask(void);
char Get_SNInfo_Fun(void);











#endif


