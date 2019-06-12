#ifndef __MODBUS_H
#define __MODBUS_H

//大小端转换
#define htons(n) ((((n) & 0x00ff) << 8) | (((n) & 0xff00) >> 8))
#define ntohs(n) htons(n)
#define htonl(n) ((((n) & 0x000000ff) << 24)|  \
				 (((n) & 0x0000ff00) << 8)  |  \
				 (((n) & 0x00ff0000) >> 8)  |  \
				 (((n) & 0xff000000) >> 24))
#define ntohl(n) htonl(n)
#define htond(n) (htonl(n & 0xffffffff) << 32) | htonl(n >> 32)
#define ntohd(n) htond(n)

typedef union{													 //float型数据大小端格式转换
	float f;
	char c[4];
}FLOAT_CONV;


/* Exported types ------------------------------------------------------------*/
//modbus 功能码
enum ModbusFunction_t
{
	ReadKeepRegistor=0x03,        //读取保持寄存器
	ReadInputRegistor=0x04,       //读取输入寄存器
	WriteSingleRegistor=0x06,     //预置单寄存器
	WriteSomeRegistor=0x10,       //预置多寄存器
	//Get_SNInfo=0x40,            //获取SN信息
	HMC5883SetOffset=0x40,	      //自动设置初值
	SetOriginalVal=0x41,	        //自动设置初值
	StartSampling= 0x42,          //采样
	FactoryParameterReset=0x68,   //恢复出厂设置
};

enum err_num  {
	err_Fu=1,           //非支持的功能（功能码异常）
	err_add,            //寄存器地址不正确
	err_Re_VOR,         //寄存器值超出范围
	err_OE,             //有效操作发生异常
	//err_CON,          //确认
	//err_Fu_NUM,       //功能号无效
	//err_busy          //设备忙
};


//保持寄存器结构体
typedef struct __attribute__ ((__packed__))
{
	unsigned short DeviceAddress;					//设备地址
	unsigned short DeviceMode;						//设备工作模式
	unsigned short IS_USER_MAG;						//液位初始高度高位
	float	   OriginalPitch;					//初始Pitch
	float	   OriginalRoll;					//初始Roll
	float	   OriginalAzimuth;				//初始Azimuth
	short	   MAG_Mx;					//半轴长
	short	   MAG_My;					//半轴长
	short	   MAG_Mz;					//半轴长
	float    MAG_X_OFFSET;		//磁力计X轴偏移量
	float    MAG_Y_OFFSET;		//磁力计Y轴偏移量
	float    MAG_Z_OFFSET;		//磁力计Z轴偏移量

	unsigned short DeviceLen;	//单节设备长度，单位分米
	
	float		OriginalX;
	float		OriginalY;
	float		OriginalZ;
	
}KeepRegister_type;

//输入寄存器结构体
typedef struct __attribute__ ((__packed__))
{
	unsigned short DeviceType;					//设备类型
	unsigned short SoftwareVersion;			//软件版本
	unsigned int SystemWorkTime;			//系统工作时间
	unsigned short SystemWorkStatus;		//系统工作状态
	float	   XACC;								//ACC X
	float	   YACC;								//ACC Y
	float	   ZACC;								//ACC Z
	float	   TACC;								//ACC T
	float	   XMAG;								//MAG Z轴
	float	   YMAG;								//MAG Z轴
	float	   ZMAG;								//MAG Z轴
	unsigned short   Len;
	unsigned short   SN[8];
	float    pitch;
	float    roll;
	float    azimuth;
	float	   OriginalPitch;					//初始Pitch
	float	   OriginalRoll;					//初始Roll
	float	   OriginalAzimuth;					//初始Azimuth
	
	float		X;				//X轴数据
	float		Y;				//Y轴数据
	float		Z;				//Z轴数据
	
	float		OriginalX;//初始X轴数据
	float		OriginalY;//初始Y轴数据
	float		OriginalZ;//初始Z轴数据
	
	
}InputRegister_type;


//modbus 消息接收结构体
typedef struct __attribute__ ((__packed__))
{
	unsigned char DataFlag;					//数据标记,是否有数据
	unsigned char DataLen;					//数据长度
	unsigned char dat[UART1_RBUF_SIZE];	    //数据缓存
}ModbusDataPackage_type;


extern volatile KeepRegister_type		KeepRegister;		//定义保持寄存器
extern volatile InputRegister_type		InputRegister;		//定义输入寄存器
extern volatile ModbusDataPackage_type ModbusDataPackage;	//定义modbus接收缓存

extern volatile unsigned char Modbus_BusyFlag;
extern volatile unsigned char Modbus_FreeFlag;
static float __ltobf(float data);
unsigned short CRC16_Check(unsigned char *Pushdata,unsigned char length);
void ModbusReturnAckInfo(unsigned char err);
void KeepRegistorDataHton(void);
void InputRegistorDataHton(void);
char ModbusReadKeepRegistor(void);
char ModbusReadInputRegistor(void);
char ModbusWriteSingleRegistor(void);
char ModbusWriteSomeRegistor(void);
char ModbusSetInitalValue(void);
char ModbusFactoryParameterReset(void);
char Get_SNInfo_Fun(void);
char HMC5883_Offset(void);
unsigned char ProcessTask(void);
char ModbusStartSampling(void);
void Euler_Solution(float len,float pitch,float roll,float yaw,float * pr);

void InstructionTask(void);











#endif


