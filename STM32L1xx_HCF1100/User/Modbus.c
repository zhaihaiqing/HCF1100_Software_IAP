
#include "main.h"

volatile unsigned char Modbus_BusyFlag;
volatile unsigned char Modbus_FreeFlag;

const unsigned char  FactoryResetWord[4]    = {0x33,0x55,0x77,0x99};//��������ָ�����
const unsigned char  FactorySetValueWord[4] = {0x44,0x66,0x88,0xaa};//��������ָ�����

volatile KeepRegister_type				KeepRegister;		//���屣�ּĴ���
volatile KeepRegister_type				KeepRegisterTemp;	//���屣�ּĴ�������
volatile InputRegister_type	 			InputRegister;		//��������Ĵ���
volatile InputRegister_type	 			InputRegisterTemp;	//��������Ĵ�������
volatile ModbusDataPackage_type 	ModbusDataPackage;	//����modbus���ջ���


/*******************************************************************************
* Function Name  : __ltobf
* Description    : float�����ݴ�С�˸�ʽת��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static float __ltobf(float data)
{
	FLOAT_CONV d1, d2;

	d1.f = data;

	d2.c[0] = d1.c[3];
	d2.c[1] = d1.c[2];
	d2.c[2] = d1.c[1];
	d2.c[3] = d1.c[0];
	return d2.f;
}
/*******************************************************************************
* Function Name  : CRC16_Check
* Description    : CRCУ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t CRC16_Check(uint8_t *Pushdata,uint8_t length)
{
	uint16_t Reg_CRC=0xffff;
	uint8_t i,j;
	for( i = 0; i<length; i ++)
	{
		Reg_CRC^= *Pushdata++;
		for (j = 0; j<8; j++)
		{
			if (Reg_CRC & 0x0001)

			Reg_CRC=Reg_CRC>>1^0xA001;
			else
			Reg_CRC >>=1;
		}
	}
	return   Reg_CRC;
}

/*******************************************************************************
* Function Name  : ModbusReturnAckInfo
* Description    : modbus�����쳣����Ϣ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ModbusReturnAckInfo(uint8_t err)
{
	uint8_t ErrDat[5];
	uint16_t crc;
	//log_info("err:0x%x\r\n",err);
	if(err)//�쳣��
	{
		ErrDat[0] = KeepRegister.DeviceAddress;//��ֵ�豸��ַ
		ErrDat[1] = ModbusDataPackage.dat[1] | 0x80;//��ֵ�쳣������,��������+0x80
		ErrDat[2] = err;//��ֵ�쳣��
		crc = CRC16_Check(ErrDat,3);
		ErrDat[3] = (crc & 0xff);//У���8λ
		ErrDat[4] = (crc >> 8);//У���8λ
		
		U485SendData(ErrDat,5);//��485��������
		//err=0;
	}
}

/*******************************************************************************
* Function Name  : KeepRegistorDataHton
* Description    : ���ּĴ������ݴ�С��ת��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void KeepRegistorDataHton(void)
{
	KeepRegisterTemp.DeviceAddress   = htons(KeepRegister.DeviceAddress);
	KeepRegisterTemp.DeviceMode      = htons(KeepRegister.DeviceMode);
	KeepRegisterTemp.IS_USER_MAG     = htons(KeepRegister.IS_USER_MAG);
	KeepRegisterTemp.OriginalPitch   = __ltobf(KeepRegister.OriginalPitch);
	KeepRegisterTemp.OriginalRoll    = __ltobf(KeepRegister.OriginalRoll);
	KeepRegisterTemp.OriginalAzimuth = __ltobf(KeepRegister.OriginalAzimuth);
	KeepRegisterTemp.MAG_Mx 				 = htons(KeepRegister.MAG_Mx);
	KeepRegisterTemp.MAG_My 				 = htons(KeepRegister.MAG_My);
	KeepRegisterTemp.MAG_Mz 				 = htons(KeepRegister.MAG_Mz);
	KeepRegisterTemp.MAG_X_OFFSET 	 = __ltobf(KeepRegister.MAG_X_OFFSET);
	KeepRegisterTemp.MAG_Y_OFFSET 	 = __ltobf(KeepRegister.MAG_Y_OFFSET);
	KeepRegisterTemp.MAG_Z_OFFSET 	 = __ltobf(KeepRegister.MAG_Z_OFFSET);
	
	KeepRegisterTemp.DeviceLen       = htons(KeepRegister.DeviceLen);
	
	KeepRegisterTemp.OriginalX 		   = __ltobf(KeepRegister.OriginalX);
	KeepRegisterTemp.OriginalY       = __ltobf(KeepRegister.OriginalY);
	KeepRegisterTemp.OriginalZ       = __ltobf(KeepRegister.OriginalZ);
	
}
/*******************************************************************************
* Function Name  : InputRegistorDataHton
* Description    : ����Ĵ������ݴ�С��ת��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InputRegistorDataHton(void)
{
	unsigned char i;
	InputRegisterTemp.DeviceType 						=	htons(InputRegister.DeviceType);
	InputRegisterTemp.SoftwareVersion 			=	htons(InputRegister.SoftwareVersion);
	InputRegisterTemp.SystemWorkTime 				=	htonl(InputRegister.SystemWorkTime);
	InputRegisterTemp.SystemWorkStatus 			=	htons(InputRegister.SystemWorkStatus);
	InputRegisterTemp.XACC 									=	__ltobf(InputRegister.XACC);
	InputRegisterTemp.YACC 									=	__ltobf(InputRegister.YACC);
	InputRegisterTemp.ZACC 									=	__ltobf(InputRegister.ZACC);
	InputRegisterTemp.TACC 									=	__ltobf(InputRegister.TACC);
	
	InputRegisterTemp.XMAG 									=	__ltobf(InputRegister.XMAG);
	InputRegisterTemp.YMAG 									=	__ltobf(InputRegister.YMAG);
	InputRegisterTemp.ZMAG 									=	__ltobf(InputRegister.ZMAG);
	InputRegisterTemp.Len 									= htons(InputRegister.Len);
	for(i=0;i<8;i++)InputRegisterTemp.SN[i] =	htons(InputRegister.SN[i]);
	//for(i=0;i<8;i++)InputRegisterTemp.SN[i] =	InputRegister.SN[i];
	InputRegisterTemp.pitch 								=	__ltobf(InputRegister.pitch);
	InputRegisterTemp.roll 									=	__ltobf(InputRegister.roll);
	InputRegisterTemp.azimuth 							=	__ltobf(InputRegister.azimuth);
	InputRegisterTemp.OriginalPitch 				=	__ltobf(InputRegister.OriginalPitch);
	InputRegisterTemp.OriginalRoll 					=	__ltobf(InputRegister.OriginalRoll);
	InputRegisterTemp.OriginalAzimuth 			=	__ltobf(InputRegister.OriginalAzimuth);
	
	InputRegisterTemp.X 										=	__ltobf(InputRegister.X);
	InputRegisterTemp.Y 										=	__ltobf(InputRegister.Y);
	InputRegisterTemp.Z 										=	__ltobf(InputRegister.Z);
	
	InputRegisterTemp.OriginalX							=	__ltobf(InputRegister.OriginalX);
	InputRegisterTemp.OriginalY 						=	__ltobf(InputRegister.OriginalY);
	InputRegisterTemp.OriginalZ 						=	__ltobf(InputRegister.OriginalZ);
}
/********************************
�����ּĴ���
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x03)+��ʼ��ַ(2�ֽ�)+��ȡ�Ĵ�������(2�ֽ�)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x03)+���ݳ���(�ֽ���,1�ֽ�)+����(n�ֽ�)+У��(2�ֽ�)
*********************************/
char ModbusReadKeepRegistor(void)
{
	unsigned char err = 0;
	uint8_t temp[180];
	uint16_t crc;
	
 	uint16_t StartAddress = (ModbusDataPackage.dat[2] << 8) | ModbusDataPackage.dat[3]; //��ȡ��ʼ��ַ
 	uint16_t RegVal = (ModbusDataPackage.dat[4] << 8) | ModbusDataPackage.dat[5];		//��ȡҪ��ȡ�ļĴ�������
 	uint16_t StopAddress = StartAddress + RegVal - 1;									//��ȡ������ַ
 	uint16_t KeepRegistorSize = sizeof(KeepRegister) / 2;								//���㱣�ּĴ���������
 	uint16_t bytes = RegVal*2;															//����Ҫ��ȡ���ֽ���
	
 	if(ModbusDataPackage.DataLen != 8)err = err_OE;										//��Ч���������쳣
 	if(!RegVal)err = err_add;															//�Ĵ�����ַ����ȷ,��ȡ�����������1
 	if( (  (StartAddress == 0x04) || (StartAddress == 0x06) || (StartAddress == 0x08) || \
				 (StartAddress == 0x0D) || (StartAddress == 0x0F) || (StartAddress == 0x11) || \
				 (StartAddress == 0x14) || (StartAddress == 0x16) || (StartAddress == 0x18) ) )err = err_add;//�Ĵ�����ַ����ȷ,���ֽ�����Ҫ��ͷ����
 	if( (  (StopAddress == 0x03)  || (StopAddress == 0x05)  || (StopAddress == 0x07)  || \
				 (StopAddress == 0x0C)  || (StopAddress == 0x0E)  || (StopAddress == 0x10)  || \
	       (StopAddress == 0x13)  || (StopAddress == 0x15)  || (StopAddress == 0x17)  ) )err = err_add;//�Ĵ�����ַ����ȷ,���ֽ�����û��ȫ������
 	if(StopAddress > (KeepRegistorSize - 1))err = err_add;							//�Ĵ�����ַ����ȷ
 	if(StartAddress > (KeepRegistorSize - 1))err = err_add;							//�Ĵ�����ַ����ȷ
	 
	if(  err != 0  )							//�����쳣����Ϣ
	{
		ModbusReturnAckInfo(err);														//��485�����쳣����Ϣ
		return ERROR;
	}
 	
 	KeepRegistorDataHton();																//��С�����ݴ���,���ڻ�����
 	/*��ȡ���ּĴ������ݲ���485��������*/
  	
	  temp[0] = KeepRegister.DeviceAddress;								//��ֵ�豸��ַ
  	temp[1] = ModbusDataPackage.dat[1];									//��ֵ������
  	temp[2] = bytes;													//��ֵ���ݳ���(�ֽ���)
  	memcpy(&temp[3],(uint8_t *)&KeepRegisterTemp+StartAddress*2,bytes);	//�ַ���copy
  	
  	crc=CRC16_Check(temp,bytes+3);										//ִ��crcУ��
  	temp[bytes+3]=crc & 0xff;
  	temp[bytes+4]=crc>>8;
 	if(ModbusDataPackage.dat[0]) U485SendData(temp,bytes+5);			//��������
	
	return SUCCESS;	
}

/********************************
������Ĵ���
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x04)+��ʼ��ַ(2�ֽ�)+��ȡ�Ĵ�������(2�ֽ�)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x04)+���ݳ���(�ֽ���,1�ֽ�)+����(n�ֽ�)+У��(2�ֽ�)
*********************************/
char ModbusReadInputRegistor(void)
{
	uint8_t err=0;
	uint8_t temp[180];
	uint16_t crc;
	
	uint16_t StartAddress = (ModbusDataPackage.dat[2] << 8) | ModbusDataPackage.dat[3];	//��ȡ��ʼ��ַ
	uint16_t RegVal = (ModbusDataPackage.dat[4] << 8) | ModbusDataPackage.dat[5];		//��ȡҪ��ȡ�ļĴ�������
	uint16_t StopAddress = StartAddress + RegVal - 1;									//��ȡ������ַ
	uint16_t InputRegistorSize = sizeof(InputRegister) / 2;								//���㱣�ּĴ���������
	uint16_t bytes = RegVal*2;															//����Ҫ��ȡ���ֽ���
	
	InputRegister.OriginalPitch = KeepRegister.OriginalPitch;
	InputRegister.OriginalRoll = KeepRegister.OriginalRoll;
	InputRegister.OriginalAzimuth = KeepRegister.OriginalAzimuth;
	
	if(ModbusDataPackage.DataLen != 8)err = err_OE;										//��Ч���������쳣
	if(!RegVal)err = err_add;															//�Ĵ�����ַ����ȷ,��ȡ�����������1
	if( ( (StartAddress==0x03) || (StartAddress==0x06) || (StartAddress==0x08) || \
		    (StartAddress==0x0A) || (StartAddress==0x0C) || (StartAddress==0x0e) || \
		    (StartAddress==0x10) || (StartAddress==0x12) || (StartAddress==0x1d) || \
	      (StartAddress==0x1f) || (StartAddress==0x21) || (StartAddress==0x23) || \
				(StartAddress==0x25) || (StartAddress==0x27) || (StartAddress==0x29) || \
				(StartAddress==0x2B) || (StartAddress==0x2D) || (StartAddress==0x2F) || \
	      (StartAddress==0x31) || (StartAddress==0x33)                           ) )err = err_add;//�Ĵ�����ַ����ȷ,���ֽ�����Ҫ��ͷ����
	if( ( (StopAddress==0x02)  || (StopAddress==0x05)  || (StopAddress==0x07)  || \
				(StopAddress==0x09)  || (StopAddress==0x0B)  || (StopAddress==0x0d)  || \
				(StopAddress==0x0f)  || (StopAddress==0x11)  || (StopAddress==0x1c)  || \
				(StopAddress==0x1e)  || (StopAddress==0x20)  || (StopAddress==0x22)  || \
				(StopAddress==0x24)  || (StopAddress==0x26)  ||	(StopAddress==0x28)  || \
				(StopAddress==0x2A)  || (StopAddress==0x2C)	 || (StopAddress==0x2E)  || \
				(StopAddress==0x30)  || (StopAddress==0x32)	 										       ) )err = err_add;//�Ĵ�����ַ����ȷ,���ֽ�����û��ȫ������
	if(StopAddress > (InputRegistorSize - 1))err = err_add;	//�Ĵ�����ַ����ȷ
	if(StartAddress > (InputRegistorSize - 1))err = err_add;	//�Ĵ�����ַ����ȷ
	if(  ((ModbusDataPackage.dat[4]<<8) | ModbusDataPackage.dat[5]) > InputRegistorSize    )err = err_Re_VOR;
	if(  err != 0 )						//�����쳣����Ϣ
	{
		ModbusReturnAckInfo(err);								//��485�����쳣����Ϣ
		return ERROR;
	}
	
	InputRegistorDataHton();									//��С�����ݴ���,���ڻ�����
	//��ȡ����Ĵ������ݲ���485��������
	
	temp[0] = KeepRegister.DeviceAddress;						//��ֵ�豸��ַ
	temp[1] = ModbusDataPackage.dat[1];							//��ֵ������
	temp[2] = bytes;											//��ֵ���ݳ���(�ֽ���)
	memcpy(&temp[3],(uint8_t *)&InputRegisterTemp+StartAddress*2,bytes);
	
	crc=CRC16_Check(temp,bytes+3);
	temp[bytes+3]=crc & 0xff;
	temp[bytes+4]=crc>>8;
	if(ModbusDataPackage.dat[0]) U485SendData(temp,bytes+5);
	return SUCCESS;	
}

/********************************
д�����Ĵ���,���ּĴ���
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x06)+�Ĵ�����ַ(2�ֽ�)+�Ĵ�����ֵ(2�ֽ�)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x06)+�Ĵ�����ַ(2�ֽ�)+�Ĵ�����ֵ(2�ֽ�)+У��(2�ֽ�)
*********************************/
char ModbusWriteSingleRegistor(void)
{
	uint8_t err=0;
	uint8_t temp[10];
	uint16_t crc,dat;
	//��ȡ��ز���
	uint16_t StartAddress = (ModbusDataPackage.dat[2] << 8) | ModbusDataPackage.dat[3];	//��ȡ��ʼ��ַ
	dat = (ModbusDataPackage.dat[4] << 8) | ModbusDataPackage.dat[5];					//��ȡҪд�������
	
	//�����Ϸ����
	if(ModbusDataPackage.DataLen != 8)err = err_OE;	//��Ч���������쳣
	if( (StartAddress != 0)&&(StartAddress != 1) && (StartAddress != 2) && (StartAddress != 9) && (StartAddress != 0x0a) && (StartAddress != 0x0b) && (StartAddress != 0x12) )err = err_add;			//�쳣��,�Ĵ�����ʼ��ַ����ȷ,���ֽ����ݲ����ô˹�����
	//������Ч��Χ�жϲ�д��
	switch(StartAddress)
	{
		case 0x00:																				//���д�豸��ַ����
					if((dat == 0) || (dat > 247))err = err_OE;	//��ַ���ݳ�����Χ,�����쳣������,�Ĵ���ֵ������Χ
					else
					{
						KeepRegister.DeviceAddress = dat;
						EEWrite(KREEPROM_BASEADDR,(void *)&dat,2);//��������
					}
					break;
		case 0x01:
					KeepRegister.DeviceMode = dat;
					EEWrite(KREEPROM_BASEADDR+2*0x01,(void *)&dat,2);//��������
					break;
		
		case 0x02:
					KeepRegister.IS_USER_MAG = dat;
					EEWrite(KREEPROM_BASEADDR+2*0x02,(void *)&dat,2);//��������
					break;
		
		case 0x09:
					KeepRegister.MAG_Mx = dat;
					EEWrite(KREEPROM_BASEADDR+2*0x09,(void *)&dat,2);//��������
					break;
		
		case 0x0a:
					KeepRegister.MAG_My = dat;
					EEWrite(KREEPROM_BASEADDR+2*0x0a,(void *)&dat,2);//��������
					break;
		
		case 0x0b:
					KeepRegister.MAG_Mz = dat;
					EEWrite(KREEPROM_BASEADDR+2*0x0b,(void *)&dat,2);//��������
					break;
		
		case 0x12:
					KeepRegister.DeviceLen = dat;
					InputRegister.Len = KeepRegister.DeviceLen;
					EEWrite(KREEPROM_BASEADDR+2*0x12,(void *)&dat,2);//��������
					break;
		default:
					err=err_OE;			
					break;
	}
	
	if(  err != 0 )			//�����쳣����Ϣ
	{
		ModbusReturnAckInfo(err);					//��485�����쳣����Ϣ
		return ERROR;
	}
	//ָ���
	
	temp[0] = KeepRegister.DeviceAddress;			//��ֵ�豸��ַ
	temp[1] = ModbusDataPackage.dat[1];				//��ֵ������
	memcpy(&temp[2],(uint8_t *)&ModbusDataPackage.dat[2],4);
	//У��
	crc = CRC16_Check(temp,6);						//crcУ��
	temp[6] = crc & 0xff;							//crc��λ��ǰ
	temp[7] = crc >> 8;								//��λ�ں�
	if(ModbusDataPackage.dat[0]) U485SendData(temp,8);//��������
	
	return SUCCESS;
}
/********************************
д����Ĵ���,���ּĴ���
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x10)+�Ĵ�����ַ(2�ֽ�)+�Ĵ�������(2�ֽ�)+�ֽ���(1�ֽ�)+�Ĵ�����ֵ(N������)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x10)+�Ĵ�����ַ(2�ֽ�)+�Ĵ�������(2�ֽ�)+У��(2�ֽ�)
*********************************/
char ModbusWriteSomeRegistor(void)
{
	uint8_t err=0;
	uint8_t temp[10];
	//uint8_t t[180];
	uint16_t crc;
	//��ȡ��ز���
	uint16_t StartAddress = (ModbusDataPackage.dat[2] << 8) | ModbusDataPackage.dat[3];	//��ȡ��ʼ��ַ
	uint16_t RegVal = (ModbusDataPackage.dat[4] << 8) | ModbusDataPackage.dat[5];		//��ȡҪд��ļĴ�������
	uint8_t bytes = ModbusDataPackage.dat[6];											//��ȡ�ֽ���
	uint16_t StopAddress = StartAddress + RegVal - 1;									//��ȡ������ַ
	uint16_t KeepRegistorSize = sizeof(KeepRegister) / 2;								//���㱣�ּĴ���������
	
	//U485TX;Delay_ms(10);
	
	//printf("����ֵ:mm  ������߶�ֵ:mm\r\n");
	//Delay_ms(100);U485RX;Delay_ms(10);
	
	//�����Ϸ����
	if(!RegVal || !bytes)err = err_add;													//�Ĵ�����ַ����ȷ,��ȡ�����������1
	if( (  (StartAddress == 0x04) || (StartAddress == 0x06) || (StartAddress == 0x08) || \
				 (StartAddress == 0x0D) || (StartAddress == 0x0F) || (StartAddress == 0x11) || \
				 (StartAddress == 0x14) || (StartAddress == 0x16) || (StartAddress == 0x18) ) )err = err_add;//�Ĵ�����ַ����ȷ,���ֽ�����Ҫ��ͷ����
 	if( (  (StopAddress == 0x03)  || (StopAddress == 0x05)  || (StopAddress == 0x07)  || \
				 (StopAddress == 0x0C)  || (StopAddress == 0x0E)  || (StopAddress == 0x10)  || \
	       (StopAddress == 0x13)  || (StopAddress == 0x15)  || (StopAddress == 0x17)  ) )err = err_add;//�Ĵ�����ַ����ȷ,���ֽ�����û��ȫ������
	if(StopAddress > (KeepRegistorSize - 1))err = err_add;		//�Ĵ�����ַ����ȷ
	if(StartAddress > (KeepRegistorSize - 1))err = err_add;		//�Ĵ�����ַ����ȷ
	
	
	if(  err != 0 )							//�����쳣����Ϣ
	{
		ModbusReturnAckInfo(err);									//��485�����쳣����Ϣ
		return ERROR;
	}
	
	memcpy((uint8_t *)&KeepRegisterTemp,(uint8_t *)&KeepRegister,sizeof(KeepRegister));					//�Ƚ���ǰ�Ĵ������ݿ�����������
	memcpy((uint8_t *)&KeepRegisterTemp + StartAddress*2,(uint8_t *)&ModbusDataPackage.dat[7],bytes);	//Ȼ�󽫽��յ����ݿ������Ĵ���������
	err = 0;
	//��С��ת��
	if(StartAddress == 0x00)KeepRegisterTemp.DeviceAddress = htons(KeepRegisterTemp.DeviceAddress);														//���豸��ַ���ݴ�С��ת��
	if((StartAddress <= 0x01) && (StopAddress >= 0x01))KeepRegisterTemp.DeviceMode = htons(KeepRegisterTemp.DeviceMode );										//����������1���ݴ�С��ת��
	if((StartAddress <= 0x02) && (StopAddress >= 0x02))KeepRegisterTemp.IS_USER_MAG = htons(KeepRegisterTemp.IS_USER_MAG );										//����������1���ݴ�С��ת��
	if((StartAddress <= 0x03) && (StopAddress >= 0x04))KeepRegisterTemp.OriginalPitch = __ltobf(KeepRegisterTemp.OriginalPitch);				
	if((StartAddress <= 0x05) && (StopAddress >= 0x06))KeepRegisterTemp.OriginalRoll = __ltobf(KeepRegisterTemp.OriginalRoll );						
	if((StartAddress <= 0x07) && (StopAddress >= 0x08))KeepRegisterTemp.OriginalAzimuth = __ltobf(KeepRegisterTemp.OriginalAzimuth );	
	
	if((StartAddress <= 0x09) && (StopAddress >= 0x09))KeepRegisterTemp.MAG_Mx = htons(KeepRegisterTemp.MAG_Mx);				
	if((StartAddress <= 0x0a) && (StopAddress >= 0x0a))KeepRegisterTemp.MAG_My = htons(KeepRegisterTemp.MAG_My );						
	if((StartAddress <= 0x0b) && (StopAddress >= 0x0b))KeepRegisterTemp.MAG_Mz = htons(KeepRegisterTemp.MAG_Mz );	
	
	if((StartAddress <= 0x0c) && (StopAddress >= 0x0d))KeepRegisterTemp.MAG_X_OFFSET = __ltobf(KeepRegisterTemp.MAG_X_OFFSET );		
	if((StartAddress <= 0x0e) && (StopAddress >= 0x0f))KeepRegisterTemp.MAG_Y_OFFSET = __ltobf(KeepRegisterTemp.MAG_Y_OFFSET );				
	if((StartAddress <= 0x10) && (StopAddress >= 0x11))KeepRegisterTemp.MAG_Z_OFFSET = __ltobf(KeepRegisterTemp.MAG_Z_OFFSET );
	
	
	if((StartAddress <= 0x12) && (StopAddress >= 0x12))KeepRegisterTemp.DeviceLen = htons(KeepRegisterTemp.DeviceLen );										//����������1���ݴ�С��ת��

	if((StartAddress <= 0x14) && (StopAddress >= 0x13))KeepRegisterTemp.OriginalX = __ltobf(KeepRegisterTemp.OriginalX );
	if((StartAddress <= 0x16) && (StopAddress >= 0x15))KeepRegisterTemp.OriginalY = __ltobf(KeepRegisterTemp.OriginalY );				
	if((StartAddress <= 0x18) && (StopAddress >= 0x17))KeepRegisterTemp.OriginalZ = __ltobf(KeepRegisterTemp.OriginalZ );
	
	
	//�ж�������Ч��
	if((KeepRegisterTemp.DeviceAddress == 0) || (KeepRegisterTemp.DeviceAddress > 247))err = err_Re_VOR;	//��ַ���ݳ�����Χ,�����쳣������,�Ĵ���ֵ������Χ

	if(!err)																						//����޴���,�򽫻�������ݿ������Ĵ�����
	{
		memcpy((uint8_t *)&KeepRegister,(uint8_t *)&KeepRegisterTemp,sizeof(KeepRegister));
		EEWrite(KREEPROM_BASEADDR,(void *)&KeepRegisterTemp,sizeof(KeepRegister));					//�������ݣ����������Ĵ�����
		if((StartAddress <= 0x12) && (StopAddress >= 0x12))InputRegister.Len = KeepRegister.DeviceLen;
	}
	else if(ModbusDataPackage.dat[0])//������ݷ�Χ����
	{
		ModbusReturnAckInfo(err);	 //��485�����쳣����Ϣ
		return ERROR;
	}
	
	//ָ���
	temp[0] = KeepRegister.DeviceAddress;					//��ֵ�豸��ַ
	temp[1] = ModbusDataPackage.dat[1];						//��ֵ������
	memcpy(&temp[2],(uint8_t *)&ModbusDataPackage.dat[2],4);//��ֵ�Ĵ�����ַ������,��4�ֽ�
	//У��
	crc = CRC16_Check(temp,6);								//crcУ��
	temp[6] = crc & 0xff;									//crc��λ��ǰ
	temp[7] = crc >> 8;										//��λ�ں�
	if(ModbusDataPackage.dat[0]) U485SendData(temp,8);		//��������
	
	return SUCCESS;
}

/********************************
�趨��ֵ����ɺ�д���ּĴ���
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x41)+������(4�ֽ�)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x41)+���޸ļĴ����׵�ַ(2�ֽ�)+���޸ļĴ�������(2�ֽ�)+У��(2�ֽ�)
������:0x44,0x66,0x88,0xaa
*********************************/
char ModbusSetInitalValue(void)
{
	uint8_t err=0;
	uint8_t temp[10];
	uint16_t crc;
	float OriginaAltitudeTemp=0;
	if(ModbusDataPackage.DataLen !=8 )err = err_OE;
	if( strncmp(FactorySetValueWord,(unsigned char *)&ModbusDataPackage.dat[2],4) !=0 )err=err_OE;
	if(  (err != 0)  &&  (ModbusDataPackage.dat[0] != 0) )
	{
		ModbusReturnAckInfo(err);
		return ERROR;
	}
	
	KeepRegister.OriginalX = InputRegister.X; 
	KeepRegister.OriginalY = InputRegister.Y;
	KeepRegister.OriginalZ = InputRegister.Z;
	
	KeepRegister.OriginalPitch = InputRegister.pitch;   //��ȡ��ǰ��̬��
	KeepRegister.OriginalRoll = InputRegister.roll;   
	KeepRegister.OriginalAzimuth = InputRegister.azimuth;    
	
	EEWrite(KREEPROM_BASEADDR+6,(void *)&KeepRegister.OriginalPitch,12);    //ͬʱ�����ݱ�����EEPROM
	
	EEWrite(KREEPROM_BASEADDR+38,(void *)&KeepRegister.OriginalX,12);    //ͬʱ�����ݱ�����EEPROM
	
	temp[0] = KeepRegister.DeviceAddress;
	temp[1] = ModbusDataPackage.dat[1];
	temp[2] = 0x00;
	temp[3] = 0x00;
	temp[4] = 0x00;
	temp[5] = 0x00;
	
	crc = CRC16_Check(temp,6);
	
	temp[6] = crc &0xff;
	temp[7] = crc >> 8;
	if(ModbusDataPackage.dat[0]) U485SendData(temp,8);
	
	return SUCCESS;
}

/********************************
��������Բ��������
�յ�ָ���������������������ԭʼ�������ݷ�������λ��
*********************************/
char HMC5883_Offset(void)
{
	uint8_t err=0;
	uint8_t temp[20];
	uint16_t crc;
	unsigned short i;
	unsigned short count=0;
	signed short MAG_data[3];
	
	if(ModbusDataPackage.DataLen !=8 )err = err_OE;
	if( strncmp(FactorySetValueWord,(unsigned char *)&ModbusDataPackage.dat[2],4) !=0 )err=err_OE;
	if(  (err != 0)  &&  (ModbusDataPackage.dat[0] != 0) )
	{
		ModbusReturnAckInfo(err);
		return ERROR;
	}
	//����8��
	for(i=0;i<300;i++)
	{
		HMC5883L_SetMode();
		ClC_WatchDogTask();											//���Ź�����
		HMC5883L_Task(MAG_data);								//�����Ʋ������񣬻�ȡ����������
		Delay(50);
		ClC_WatchDogTask();											//���Ź�����
		count++;
		
		temp[0] = KeepRegister.DeviceAddress;
		temp[1] = ModbusDataPackage.dat[1];
		temp[2] = 0x00;
		temp[3] = 0x02;
		temp[4] = 0x00;
		
		temp[5] = (count&0xff00)>>8;
		temp[6] = count&0x00ff;
		
		temp[7] = (MAG_data[0] & 0xff00)>>8;
		temp[8] = (MAG_data[0] & 0x00ff);
		temp[9] = (MAG_data[1] & 0xff00)>>8;
		temp[10] = (MAG_data[1] & 0x00ff);
		temp[11] = (MAG_data[2] & 0xff00)>>8;
		temp[12] = (MAG_data[2] & 0x00ff);
		
		crc = CRC16_Check(temp,13);
		
		temp[13] = crc &0xff;
		temp[14] = crc >> 8;
		//log_info("[%d]Xmag:%d,Ymag:%d,Zmag:%d\r\n",i,MAG_data[0],MAG_data[1],MAG_data[2]);
		if(ModbusDataPackage.dat[0]) U485SendData(temp,15);
	}
	return 1;
}

/********************************
�趨��ֵ����ɺ�д���ּĴ���
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x41)+������(4�ֽ�)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x41)+���޸ļĴ����׵�ַ(2�ֽ�)+���޸ļĴ�������(2�ֽ�)+У��(2�ֽ�)
������:0x33,0x55,0x77,0x99
*********************************/
char ResetSoftware(void)
{
	uint8_t err=0;
	uint8_t temp[10];
	uint16_t crc;
	float OriginaAltitudeTemp=0;
	if(ModbusDataPackage.DataLen !=8 )err = err_OE;
	if( strncmp(FactoryResetWord,(unsigned char *)&ModbusDataPackage.dat[2],4) !=0 )err=err_OE;
	if(  (err != 0)  &&  (ModbusDataPackage.dat[0] != 0) )
	{
		ModbusReturnAckInfo(err);
		return ERROR;
	}
	
	temp[0] = KeepRegister.DeviceAddress;
	temp[1] = ModbusDataPackage.dat[1];
	temp[2] = 0x00;
	temp[3] = 0x00;
	temp[4] = 0x00;
	temp[5] = 0x00;
	
	crc = CRC16_Check(temp,6);
	
	temp[6] = crc &0xff;
	temp[7] = crc >> 8;
	if(ModbusDataPackage.dat[0]) U485SendData(temp,8);
	
	SystemResetSoft();
	
	
	return SUCCESS;
}


/*******************************************************************************
* Function Name  : ProcessTask
* Description    : ָ����������ݽ���ָ�ִͬ����Ӧ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InstructionTask(void)
{
	unsigned char ix;
	unsigned short crc;
	RS485_RX();
	if(ModbusDataPackage.DataFlag)		  //���ݰ��ѽ������
	{ 	
		for(ix=0;ix<ModbusDataPackage.DataLen;ix++)
		{
			ModbusDataPackage.dat[ix] =USART1_GetChar();//���������ݷŵ�ָ��buf
		}
		crc = CRC16_Check((uint8_t *)ModbusDataPackage.dat,ModbusDataPackage.DataLen-2 );
		if( (( crc == ( (ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1] << 8) |    ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2])        )) \
		||  ( ( (ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1]) == 0xff    ) && ((ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2]) == 0xff))  )				
		{
			WorkTime = 0;//�����ʱʱ��
			if(ModbusDataPackage.dat[0] == KeepRegister.DeviceAddress)//�Ǳ���ָ��
			{
				switch(ModbusDataPackage.dat[1])
				{
					case ReadKeepRegistor:			//�����ּĴ���
								ModbusReadKeepRegistor();
								break;
					case ReadInputRegistor:			//������Ĵ���
								ModbusReadInputRegistor();
								InputRegister.SystemWorkStatus=0x02;
								break;
					case WriteSingleRegistor:		//д�������ּĴ���
								ModbusWriteSingleRegistor();
								InputRegister.SystemWorkStatus=0x03;
								break;
					case SoftWare_Reset:
								ResetSoftware();
								break;
					case WriteSomeRegistor:			//д������ּĴ���
								ModbusWriteSomeRegistor();
								InputRegister.SystemWorkStatus=0x04;
								break;
					case HMC5883SetOffset:		//5883У׼
								HMC5883_Offset();
								InputRegister.SystemWorkStatus=0x05;
								break;
					case SetOriginalVal:		//�趨��ʼֵ
								ModbusSetInitalValue();
								InputRegister.SystemWorkStatus=0x09;
								break;
					//case StartSampling:
								//ModbusStartSampling();
								//InputRegister.SystemWorkStatus=0x0a;
								//break;
					default:
								ModbusReturnAckInfo(err_Fu);//��485�����쳣����Ϣ,���ܺ���Ч
								break;
				}
			}
			/********************************
			�㲥ָ���֧�ָֻ���ַ��һ���������������·�޷���������
			�㲥ָ����ظ���������
			���ݸ�ʽ:������ַ(1�ֽ�)+������(0x)+������(4�ֽ�)+У��(2�ֽ�) 
			�ֶ�:0x33,0x55,0x77,0x99
			*********************************/
			else if (ModbusDataPackage.dat[0] == 0x00)//�ǹ㲥ָ��
			{

				switch(ModbusDataPackage.dat[1])
				{
					//case SetOriginalVal:		//�趨��ʼֵ
								ModbusSetInitalValue();
								InputRegister.SystemWorkStatus=0x09;
								break;
					//case StartSampling:
								//ModbusStartSampling();
								//InputRegister.SystemWorkStatus=0x0a;
								//break;
					case SoftWare_Reset:
								ResetSoftware();
								break;
					default:
								break;
				}
			} 
		}
		USART1_ClearBuf_Flag();				 //��մ��ڽ��ջ���
		ModbusDataPackage.DataLen = 0;  //����ճ��ȣ�ע�����˳��
		ModbusDataPackage.DataFlag = 0; //��ձ��λ
	}
}
/*******************************************************************************
* Function Name  : Euler_Solution
* Description    : ��̬���㺯����ŷ����ת����ʽ
* Input          : �豸����len��������Ϣ�����豸��ʼ״̬����[0;0;-len]
* Input					 : pitch�ǡ�roll�ǡ�yaw��
* Input				   : �������ָ��
* Output         : X,Y,Zֵ
* Return         : None
*******************************************************************************/
void Euler_Solution(float len,float pitch,float roll,float yaw,float * pr)
{
	float p=0,sp=0,cp=0;
	float r=0,sr=0,cr=0;
	float y=0,sy=0,cy=0;
	float m11=0,m12=0,m13=0;
	float m21=0,m22=0,m23=0;
	float m31=0,m32=0,m33=0;
	//float a1=0,a2=0;
	float a3=-len;
	
	p=pitch * PI / 180;																		//ת��Ϊ����
	r=roll  * PI / 180;																		//ת��Ϊ����
	y=yaw   * PI / 180;																		//ת��Ϊ����
	
	/* ����������Ƕȵ����ҡ�����ֵ���������Ч�� */
	sp=sin(p);
	cp=cos(p);
	sr=sin(r);
	cr=cos(r);
	sy=sin(y);
	cy=cos(y);
	
	/* ������ת����M�и�Ԫ������ */
	m11 = cy*cp;
	m12 = -sy*cr + cy*sp*sr;
	m13 = sy*sr + cy*sp*cr;
	m21 = sy*cp;
	m22 = cy*cr + sy*sp*sr;
	m23 = -cy*sr + sy*sp*cr;
	m31 = 0;
	m32 = 0;
	m33 = cp*cr;
	
	/* ��ʼ����A=[0;0;len],�任�����̬����=M��A */
	*pr     = 10   * m13 * a3;
	*(pr+1) = 10   * m23 * a3;
	*(pr+2) = 0-10 * m33 * a3;
	//log_info("X:%f,Y:%f,Z:%f\r\n",*pr ,*(pr+1),*(pr+2));
}

/*************************************************************
* * ProcessTask()��������
* *
* *
* *����NED����ϵ
*************************************************************/
unsigned char ProcessTask(void)
{
	//���㽫����NED����ϵ�����������£�
	unsigned char i=0;
	float ACC_data[4]={0};
	float MAG_data[3]={0};
	float accx=0,accy=0,accz=0,acct=0;
	float mag_xh=0,mag_yh=0;
	double ACC_pitch=0,ACC_roll=0;
	unsigned short accCount=20;//������ٶȼƲ���������20�鹲1000������
	//unsigned int tt1=0;
	//unsigned char temp[180]={0},bytes=0;
	float XYZ[3]={0};
	//uint16_t crc;

	//Sampling_tryagain:
	/*********************���ٶȼƲ�������*****************************************/
	//tt1=SysTick_Count1;
	for(i=0;i<accCount;i++)                 //��������1000�����ݷ�accCount�βɼ������ⵥһ����ʱ��ִ��
	{
		ClC_WatchDogTask();										//���Ź�ι�����񣬿��Ź���λ���Ϊ1.6S
		InstructionTask();										//ָ���������
		ADXL355_Samp50_Task(ACC_data);				//���ٶȲ������񣬲ɼ�50�����ݣ������ȡ�м�10����ƽ��������ƽ����
		accx+=ACC_data[0];
		accy+=ACC_data[1];
		accz+=ACC_data[2];
		acct+=ACC_data[3];
		//log_info("accx:%f,accy:%f,accz:%f,acct:%f\r\n",ACC_data[0],ACC_data[1],ACC_data[2],ACC_data[3]);
	}
	
	//���ټ�����ϵ�任
	IMU_Data.XACC=-accz/accCount;									//������ٶȾ�ֵ
	IMU_Data.YACC=-accx/accCount;
	IMU_Data.ZACC=-accy/accCount;
	IMU_Data.TACC= acct/accCount;      						//�����¶�����
	
	InputRegister.XACC =  IMU_Data.XACC;
	InputRegister.YACC =  IMU_Data.YACC;
	InputRegister.ZACC =  IMU_Data.ZACC;
	InputRegister.TACC =  IMU_Data.TACC;
	
	/************************���㸩���Ǻͷ�����**********************/
	ACC_roll  = atan2(IMU_Data.YACC, IMU_Data.ZACC); //���㷭����,����
	ACC_pitch = atan2(-IMU_Data.XACC, (IMU_Data.YACC*sin(ACC_roll) + IMU_Data.ZACC*cos(ACC_roll)) );//���㸩���ǣ�����
	
	InputRegister.pitch=ACC_pitch*180.0/PI;//���㸩���ǣ�ת��Ϊ�Ƕȸ�ֵ������Ĵ���
	InputRegister.roll=ACC_roll*180.0/PI;	 //�������ǣ�ת��Ϊ�Ƕȸ�ֵ������Ĵ���
	
	/*********************�����Ʋ�������*****************************************/
	if(KeepRegister.IS_USER_MAG) 					//ʹ�ô���������,����д����Ʋ���
	{
		//	do
		//	{
		//		Modbus_BusyFlag=0;		//����ģʽ��ʹ�ô����жϹ��ܼ��485����״̬���������������������
		//	}while(Modbus_FreeFlag);//���жϴ���״̬����������ݣ���������������ݣ��ȴ����ڽ������	
		//	Delay(5);	
		
		HMC5883L_MAG_TASK(MAG_data);
		
		//	Delay(5);
		//	if(Modbus_BusyFlag)
		//	{
		//		//log_info("Sampling Error!\r\n");
		//		return 0;
		//	} //goto Sampling_tryagain;			//����ģʽ��ʹ�ô����жϹ��ܼ��485����״̬���������������������
		
		//����������ϵ�任
		IMU_Data.XMAG = -MAG_data[2];
		IMU_Data.YMAG =  MAG_data[1];
		IMU_Data.ZMAG =  MAG_data[0];
	
		InputRegister.XMAG =  IMU_Data.XMAG;
		InputRegister.YMAG =  IMU_Data.YMAG;
		InputRegister.ZMAG =  IMU_Data.ZMAG;
		
		/******************�����-��б����*********************/
		//��б�����㷨
		mag_xh = 	 InputRegister.XMAG  * cos(ACC_pitch) + InputRegister.YMAG * sin(ACC_pitch) * sin(ACC_roll) + InputRegister.ZMAG * cos(ACC_roll) * sin(ACC_pitch);
		mag_yh =  -InputRegister.YMAG  * cos(ACC_roll)  + InputRegister.ZMAG * sin(ACC_roll);

		//����Ǽ���
		IMU_Data.azimuth=atan2(mag_yh, mag_xh)*180.0/PI;
		if(IMU_Data.azimuth<0)IMU_Data.azimuth=IMU_Data.azimuth+360;
		InputRegister.azimuth=IMU_Data.azimuth;
	}
	else InputRegister.azimuth=0;	     //��ʹ�ô���������,�����ֱ�Ӹ�Ϊ0

	/*   ����ŷ���ǽ�����̬����  */
	Euler_Solution(InputRegister.Len,InputRegister.pitch,InputRegister.roll,InputRegister.azimuth,XYZ);//������̬���㺯����������
	
	InputRegister.X = XYZ[0];
	InputRegister.Y = XYZ[1];
	InputRegister.Z = XYZ[2];
	
	InputRegister.OriginalX = KeepRegister.OriginalX;						//�����ּĴ����е�ԭʼXYZ���ݸ�ֵ������Ĵ���
	InputRegister.OriginalY = KeepRegister.OriginalY;
	InputRegister.OriginalZ = KeepRegister.OriginalZ;
	
	InputRegister.OriginalPitch = KeepRegister.OriginalPitch;		//�����ּĴ����е�ԭʼ�Ƕ����ݸ�ֵ������Ĵ���
	InputRegister.OriginalRoll = KeepRegister.OriginalRoll;
	InputRegister.OriginalAzimuth = KeepRegister.OriginalAzimuth;
	
//	InputRegistorDataHton();									//��С�����ݴ���,���ڻ�����
//	//��ȡ����Ĵ������ݲ���485��������
//	bytes=sizeof(InputRegister);
//	//log_info("bytes:%d\r\n",bytes);
//	temp[0] = KeepRegister.DeviceAddress;						//��ֵ�豸��ַ
//	temp[1] = 06;							//��ֵ������
//	temp[2] = bytes;											//��ֵ���ݳ���(�ֽ���)
//	memcpy(&temp[3],(uint8_t *)&InputRegisterTemp,bytes);
//	
//	crc=CRC16_Check(temp,bytes+3);
//	temp[bytes+3]=crc & 0xff;
//	temp[bytes+4]=crc>>8;
//	U485SendData(temp,bytes+5);

//log_info("Time:%d,X:%f,Y:%f,Z:%f\r\n",SysTick_Count1-tt1,InputRegister.X,InputRegister.Y,InputRegister.Z);
//log_info("Time:%d,pitch:%f,roll:%f,yaw:%f,temp:%f\r\n",InputRegister.SystemWorkTime,InputRegister.pitch,InputRegister.roll,InputRegister.azimuth,InputRegister.TACC);
	
	return 1;

}

/***********************************************************************
��ȡSN��Ϣ
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x40)+������(4�ֽ�)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x40)+��������(1�ֽ�)+SN��Ϣ(15�ֽ�)+У��(2�ֽ�)
������:0x33,0x55,0x77,0x99
************************************************************************/
//uint16_t TOS_NODE_ID=111;
//const char __progmem_smartbow_start[32] = "**SUP43214321123**4";
char Get_SNInfo_Fun(void)
{
	unsigned char i;
	//unsigned char err=0;
	//uint16_t crc;
	unsigned char SN_Info[16]={0};
	//unsigned char temp[19]={0};
	//volatile unsigned char reservalData = TOS_NODE_ID;
	
//	if(ModbusDataPackage.DataLen !=8 )err = err_OE;
//	if( strncmp(FactorySetValueWord,(unsigned char *)&ModbusDataPackage.dat[2],4) !=0 )err=err_OE;
//	
//	if(  (err != 0)  && (ModbusDataPackage.dat[0] != 0) )//���ж�
//	{
//		ModbusReturnAckInfo(err);
//		return ERROR;
//	}
	
	
	
//	
//	for(i=0; i<19; i++)
//	{
//		temp[i] = __progmem_smartbow_start[i];
//	}
//	
//	for(i=0; i<14; i++)
//	{
//		SN_Info[i] = temp[i+2];
//	}
//	SN_Info[14]=temp[18];
//	SN_Info[15]=0;
	
	EERead_Z(EEPROM_SHARE_DATA_ADDR,SN_Info,16);//��ȡEEPROM�й����ֶ��е�SN��Ϣ
	
	for(i=0;i<8;i++)
	{
		InputRegister.SN[i]=SN_Info[i*2]<<8 | SN_Info[i*2+1];
		//InputRegisterTemp.SN[i]=htons(InputRegisterTemp.SN[i]);
	}
	
	//memcpy((uint8_t *)&InputRegister,(uint8_t *)&InputRegisterTemp,sizeof(InputRegister));
	

	
//	RS485_TX();
//	for(i=0;i<8;i++)
//	{
//		InputRegisterTemp.SN[i]=SN_Info[i*2]<<8 | SN_Info[i*2+1];
//		InputRegisterTemp.SN[i]=htons(InputRegisterTemp.SN[i]);
//	}
//	log_info("SN_Info:%s\r\n",&SN_Info);
//	log_info("SN_Infi:%s\r\n",&InputRegisterTemp.SN[0]);
//	RS485_RX();
	
//	temp[0] = KeepRegister.DeviceAddress;
//	temp[1] = ModbusDataPackage.dat[1];
//	temp[2] = 0x0F;
//	
//	for(i=0;i<15;i++)
//	{
//		temp[i+3]=SN_Info[i];
//	}
//	
//	crc = CRC16_Check(temp,18);
//	
//	temp[18] = crc &0xff;
//	temp[19] = crc >> 8;
//	if(ModbusDataPackage.dat[0]) U485SendData(temp,20);
	
	return SUCCESS;
}
/********************************
�ָ���������
���ݸ�ʽ:������ַ(0)+������(0x68)+������(4�ֽ�)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x68)+�Ĵ�����ַ(2�ֽ�)+�Ĵ�������(2�ֽ�)+�ֽ���(1�ֽ�)+���б��ּĴ�������(N�ֽ�)+У��(2�ֽ�)
������:0x33,0x55,0x77,0x99
*********************************/
/*
char ModbusFactoryParameterReset(void)
{
	uint8_t err=0;
	uint8_t temp[255];
	uint16_t crc;
	if(ModbusDataPackage.DataLen != 8)err = err_OE;//��Ч���������쳣
	if(strncmp(FactoryResetWord,(char *)&ModbusDataPackage.dat[2],4) != 0)err = err_OE;//�Ĵ���ֵ������Χ
	if(  (err != 0)  && (ModbusDataPackage.dat[0] != 0) )	  //�����쳣����Ϣ
	{
		ModbusReturnAckInfo(err);			  //��485�����쳣����Ϣ
		return ERROR;
	}
	
		
	KeepRegistorDataHton();				 //��С�����ݴ���,���ڻ�����
										 
	temp[0] = KeepRegister.DeviceAddress;//��ֵ�豸��ַ
	temp[1] = ModbusDataPackage.dat[1];	 //��ֵ������
	temp[2] = 0x00;
	temp[3] = 0x00;
	temp[4] = 0x00;
	temp[5] = KeepRegister_Num;          
	temp[6] = KeepRegister_Byte_Num;
	
	memcpy(&temp[7],(uint8_t *)&KeepRegisterTemp,KeepRegister_Byte_Num);//��ȡ����
	
	//У��
	crc = CRC16_Check(temp,KeepRegister_Byte_Num+7); //crcУ��
	temp[KeepRegister_Byte_Num+7] = crc & 0xff;		//crc��λ��ǰ
	temp[KeepRegister_Byte_Num+8] = crc >> 8;		//��λ�ں�
	
	if(ModbusDataPackage.dat[0]) U485SendData(temp,KeepRegister_Byte_Num+9);//��������,����=��ȡ�ֽ���+ǰ�������ֽ�+����У�飻����ǹ㲥�����򲻻ظ���
		
	EEErase(FPOWERONFLAG_BASEADDR,2);
	//EERead(FPOWERONFLAG_BASEADDR,temp,2);
	
	SystemResetSoft();
	
	return SUCCESS;
}
*/















