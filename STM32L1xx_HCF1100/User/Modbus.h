#ifndef __MODBUS_H
#define __MODBUS_H

//��С��ת��
#define htons(n) ((((n) & 0x00ff) << 8) | (((n) & 0xff00) >> 8))
#define ntohs(n) htons(n)
#define htonl(n) ((((n) & 0x000000ff) << 24)|  \
				 (((n) & 0x0000ff00) << 8)  |  \
				 (((n) & 0x00ff0000) >> 8)  |  \
				 (((n) & 0xff000000) >> 24))
#define ntohl(n) htonl(n)
#define htond(n) (htonl(n & 0xffffffff) << 32) | htonl(n >> 32)
#define ntohd(n) htond(n)

typedef union{													 //float�����ݴ�С�˸�ʽת��
	float f;
	char c[4];
}FLOAT_CONV;


/* Exported types ------------------------------------------------------------*/
//modbus ������
enum ModbusFunction_t
{
	ReadKeepRegistor=0x03,        //��ȡ���ּĴ���
	ReadInputRegistor=0x04,       //��ȡ����Ĵ���
	WriteSingleRegistor=0x06,     //Ԥ�õ��Ĵ���
	WriteSomeRegistor=0x10,       //Ԥ�ö�Ĵ���
	//Get_SNInfo=0x40,            //��ȡSN��Ϣ
	HMC5883SetOffset=0x40,	      //�Զ����ó�ֵ
	SetOriginalVal=0x41,	        //�Զ����ó�ֵ
	StartSampling= 0x42,          //����
	FactoryParameterReset=0x68,   //�ָ���������
};

enum err_num  {
	err_Fu=1,           //��֧�ֵĹ��ܣ��������쳣��
	err_add,            //�Ĵ�����ַ����ȷ
	err_Re_VOR,         //�Ĵ���ֵ������Χ
	err_OE,             //��Ч���������쳣
	//err_CON,          //ȷ��
	//err_Fu_NUM,       //���ܺ���Ч
	//err_busy          //�豸æ
};


//���ּĴ����ṹ��
typedef struct __attribute__ ((__packed__))
{
	unsigned short DeviceAddress;					//�豸��ַ
	unsigned short DeviceMode;						//�豸����ģʽ
	unsigned short IS_USER_MAG;						//Һλ��ʼ�߶ȸ�λ
	float	   OriginalPitch;					//��ʼPitch
	float	   OriginalRoll;					//��ʼRoll
	float	   OriginalAzimuth;				//��ʼAzimuth
	short	   MAG_Mx;					//���᳤
	short	   MAG_My;					//���᳤
	short	   MAG_Mz;					//���᳤
	float    MAG_X_OFFSET;		//������X��ƫ����
	float    MAG_Y_OFFSET;		//������Y��ƫ����
	float    MAG_Z_OFFSET;		//������Z��ƫ����

	unsigned short DeviceLen;	//�����豸���ȣ���λ����
	
	float		OriginalX;
	float		OriginalY;
	float		OriginalZ;
	
}KeepRegister_type;

//����Ĵ����ṹ��
typedef struct __attribute__ ((__packed__))
{
	unsigned short DeviceType;					//�豸����
	unsigned short SoftwareVersion;			//����汾
	unsigned int SystemWorkTime;			//ϵͳ����ʱ��
	unsigned short SystemWorkStatus;		//ϵͳ����״̬
	float	   XACC;								//ACC X
	float	   YACC;								//ACC Y
	float	   ZACC;								//ACC Z
	float	   TACC;								//ACC T
	float	   XMAG;								//MAG Z��
	float	   YMAG;								//MAG Z��
	float	   ZMAG;								//MAG Z��
	unsigned short   Len;
	unsigned short   SN[8];
	float    pitch;
	float    roll;
	float    azimuth;
	float	   OriginalPitch;					//��ʼPitch
	float	   OriginalRoll;					//��ʼRoll
	float	   OriginalAzimuth;					//��ʼAzimuth
	
	float		X;				//X������
	float		Y;				//Y������
	float		Z;				//Z������
	
	float		OriginalX;//��ʼX������
	float		OriginalY;//��ʼY������
	float		OriginalZ;//��ʼZ������
	
	
}InputRegister_type;


//modbus ��Ϣ���սṹ��
typedef struct __attribute__ ((__packed__))
{
	unsigned char DataFlag;					//���ݱ��,�Ƿ�������
	unsigned char DataLen;					//���ݳ���
	unsigned char dat[UART1_RBUF_SIZE];	    //���ݻ���
}ModbusDataPackage_type;


extern volatile KeepRegister_type		KeepRegister;		//���屣�ּĴ���
extern volatile InputRegister_type		InputRegister;		//��������Ĵ���
extern volatile ModbusDataPackage_type ModbusDataPackage;	//����modbus���ջ���

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


