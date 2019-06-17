
#include "main.h"

volatile unsigned char Modbus_BusyFlag;
volatile unsigned char Modbus_FreeFlag;

const unsigned char  FactoryResetWord[4]    = {0x33,0x55,0x77,0x99};//出厂配置指令代号
const unsigned char  FactorySetValueWord[4] = {0x44,0x66,0x88,0xaa};//出厂配置指令代号

volatile KeepRegister_type				KeepRegister;		//定义保持寄存器
volatile KeepRegister_type				KeepRegisterTemp;	//定义保持寄存器缓存
volatile InputRegister_type	 			InputRegister;		//定义输入寄存器
volatile InputRegister_type	 			InputRegisterTemp;	//定义输入寄存器缓存
volatile ModbusDataPackage_type 	ModbusDataPackage;	//定义modbus接收缓存


/*******************************************************************************
* Function Name  : __ltobf
* Description    : float型数据大小端格式转换
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
* Description    : CRC校验
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
* Description    : modbus返回异常码信息
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ModbusReturnAckInfo(uint8_t err)
{
	uint8_t ErrDat[5];
	uint16_t crc;
	//log_info("err:0x%x\r\n",err);
	if(err)//异常码
	{
		ErrDat[0] = KeepRegister.DeviceAddress;//赋值设备地址
		ErrDat[1] = ModbusDataPackage.dat[1] | 0x80;//赋值异常功能码,即功能码+0x80
		ErrDat[2] = err;//赋值异常码
		crc = CRC16_Check(ErrDat,3);
		ErrDat[3] = (crc & 0xff);//校验低8位
		ErrDat[4] = (crc >> 8);//校验高8位
		
		U485SendData(ErrDat,5);//向485发送数据
		//err=0;
	}
}

/*******************************************************************************
* Function Name  : KeepRegistorDataHton
* Description    : 保持寄存器数据大小端转换
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
* Description    : 输入寄存器数据大小端转换
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
读保持寄存器
数据格式:器件地址(1字节)+功能码(0x03)+起始地址(2字节)+读取寄存器数量(2字节)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x03)+数据长度(字节数,1字节)+数据(n字节)+校验(2字节)
*********************************/
char ModbusReadKeepRegistor(void)
{
	unsigned char err = 0;
	uint8_t temp[180];
	uint16_t crc;
	
 	uint16_t StartAddress = (ModbusDataPackage.dat[2] << 8) | ModbusDataPackage.dat[3]; //获取起始地址
 	uint16_t RegVal = (ModbusDataPackage.dat[4] << 8) | ModbusDataPackage.dat[5];		//获取要读取的寄存器数量
 	uint16_t StopAddress = StartAddress + RegVal - 1;									//获取结束地址
 	uint16_t KeepRegistorSize = sizeof(KeepRegister) / 2;								//计算保持寄存器总数量
 	uint16_t bytes = RegVal*2;															//计算要读取的字节数
	
 	if(ModbusDataPackage.DataLen != 8)err = err_OE;										//有效操作发生异常
 	if(!RegVal)err = err_add;															//寄存器地址不正确,读取数量必须大于1
 	if( (  (StartAddress == 0x04) || (StartAddress == 0x06) || (StartAddress == 0x08) || \
				 (StartAddress == 0x0D) || (StartAddress == 0x0F) || (StartAddress == 0x11) || \
				 (StartAddress == 0x14) || (StartAddress == 0x16) || (StartAddress == 0x18) ) )err = err_add;//寄存器地址不正确,多字节数据要从头读出
 	if( (  (StopAddress == 0x03)  || (StopAddress == 0x05)  || (StopAddress == 0x07)  || \
				 (StopAddress == 0x0C)  || (StopAddress == 0x0E)  || (StopAddress == 0x10)  || \
	       (StopAddress == 0x13)  || (StopAddress == 0x15)  || (StopAddress == 0x17)  ) )err = err_add;//寄存器地址不正确,多字节数据没有全部读出
 	if(StopAddress > (KeepRegistorSize - 1))err = err_add;							//寄存器地址不正确
 	if(StartAddress > (KeepRegistorSize - 1))err = err_add;							//寄存器地址不正确
	 
	if(  err != 0  )							//返回异常码信息
	{
		ModbusReturnAckInfo(err);														//向485返回异常码信息
		return ERROR;
	}
 	
 	KeepRegistorDataHton();																//大小端数据处理,放在缓存中
 	/*读取保持寄存器数据并向485返回数据*/
  	
	  temp[0] = KeepRegister.DeviceAddress;								//赋值设备地址
  	temp[1] = ModbusDataPackage.dat[1];									//赋值功能码
  	temp[2] = bytes;													//赋值数据长度(字节数)
  	memcpy(&temp[3],(uint8_t *)&KeepRegisterTemp+StartAddress*2,bytes);	//字符串copy
  	
  	crc=CRC16_Check(temp,bytes+3);										//执行crc校验
  	temp[bytes+3]=crc & 0xff;
  	temp[bytes+4]=crc>>8;
 	if(ModbusDataPackage.dat[0]) U485SendData(temp,bytes+5);			//发送数据
	
	return SUCCESS;	
}

/********************************
读输入寄存器
数据格式:器件地址(1字节)+功能码(0x04)+起始地址(2字节)+读取寄存器数量(2字节)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x04)+数据长度(字节数,1字节)+数据(n字节)+校验(2字节)
*********************************/
char ModbusReadInputRegistor(void)
{
	uint8_t err=0;
	uint8_t temp[180];
	uint16_t crc;
	
	uint16_t StartAddress = (ModbusDataPackage.dat[2] << 8) | ModbusDataPackage.dat[3];	//获取起始地址
	uint16_t RegVal = (ModbusDataPackage.dat[4] << 8) | ModbusDataPackage.dat[5];		//获取要读取的寄存器数量
	uint16_t StopAddress = StartAddress + RegVal - 1;									//获取结束地址
	uint16_t InputRegistorSize = sizeof(InputRegister) / 2;								//计算保持寄存器总数量
	uint16_t bytes = RegVal*2;															//计算要读取的字节数
	
	InputRegister.OriginalPitch = KeepRegister.OriginalPitch;
	InputRegister.OriginalRoll = KeepRegister.OriginalRoll;
	InputRegister.OriginalAzimuth = KeepRegister.OriginalAzimuth;
	
	if(ModbusDataPackage.DataLen != 8)err = err_OE;										//有效操作发生异常
	if(!RegVal)err = err_add;															//寄存器地址不正确,读取数量必须大于1
	if( ( (StartAddress==0x03) || (StartAddress==0x06) || (StartAddress==0x08) || \
		    (StartAddress==0x0A) || (StartAddress==0x0C) || (StartAddress==0x0e) || \
		    (StartAddress==0x10) || (StartAddress==0x12) || (StartAddress==0x1d) || \
	      (StartAddress==0x1f) || (StartAddress==0x21) || (StartAddress==0x23) || \
				(StartAddress==0x25) || (StartAddress==0x27) || (StartAddress==0x29) || \
				(StartAddress==0x2B) || (StartAddress==0x2D) || (StartAddress==0x2F) || \
	      (StartAddress==0x31) || (StartAddress==0x33)                           ) )err = err_add;//寄存器地址不正确,多字节数据要从头读出
	if( ( (StopAddress==0x02)  || (StopAddress==0x05)  || (StopAddress==0x07)  || \
				(StopAddress==0x09)  || (StopAddress==0x0B)  || (StopAddress==0x0d)  || \
				(StopAddress==0x0f)  || (StopAddress==0x11)  || (StopAddress==0x1c)  || \
				(StopAddress==0x1e)  || (StopAddress==0x20)  || (StopAddress==0x22)  || \
				(StopAddress==0x24)  || (StopAddress==0x26)  ||	(StopAddress==0x28)  || \
				(StopAddress==0x2A)  || (StopAddress==0x2C)	 || (StopAddress==0x2E)  || \
				(StopAddress==0x30)  || (StopAddress==0x32)	 										       ) )err = err_add;//寄存器地址不正确,多字节数据没有全部读出
	if(StopAddress > (InputRegistorSize - 1))err = err_add;	//寄存器地址不正确
	if(StartAddress > (InputRegistorSize - 1))err = err_add;	//寄存器地址不正确
	if(  ((ModbusDataPackage.dat[4]<<8) | ModbusDataPackage.dat[5]) > InputRegistorSize    )err = err_Re_VOR;
	if(  err != 0 )						//返回异常码信息
	{
		ModbusReturnAckInfo(err);								//向485返回异常码信息
		return ERROR;
	}
	
	InputRegistorDataHton();									//大小端数据处理,放在缓存中
	//读取输入寄存器数据并向485返回数据
	
	temp[0] = KeepRegister.DeviceAddress;						//赋值设备地址
	temp[1] = ModbusDataPackage.dat[1];							//赋值功能码
	temp[2] = bytes;											//赋值数据长度(字节数)
	memcpy(&temp[3],(uint8_t *)&InputRegisterTemp+StartAddress*2,bytes);
	
	crc=CRC16_Check(temp,bytes+3);
	temp[bytes+3]=crc & 0xff;
	temp[bytes+4]=crc>>8;
	if(ModbusDataPackage.dat[0]) U485SendData(temp,bytes+5);
	return SUCCESS;	
}

/********************************
写单个寄存器,保持寄存器
数据格式:器件地址(1字节)+功能码(0x06)+寄存器地址(2字节)+寄存器数值(2字节)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x06)+寄存器地址(2字节)+寄存器数值(2字节)+校验(2字节)
*********************************/
char ModbusWriteSingleRegistor(void)
{
	uint8_t err=0;
	uint8_t temp[10];
	uint16_t crc,dat;
	//获取相关参数
	uint16_t StartAddress = (ModbusDataPackage.dat[2] << 8) | ModbusDataPackage.dat[3];	//获取起始地址
	dat = (ModbusDataPackage.dat[4] << 8) | ModbusDataPackage.dat[5];					//获取要写入的数据
	
	//参数合法检查
	if(ModbusDataPackage.DataLen != 8)err = err_OE;	//有效操作发生异常
	if( (StartAddress != 0)&&(StartAddress != 1) && (StartAddress != 2) && (StartAddress != 9) && (StartAddress != 0x0a) && (StartAddress != 0x0b) && (StartAddress != 0x12) )err = err_add;			//异常码,寄存器开始地址不正确,多字节数据不可用此功能码
	//数据有效范围判断并写入
	switch(StartAddress)
	{
		case 0x00:																				//如果写设备地址数据
					if((dat == 0) || (dat > 247))err = err_OE;	//地址数据超出范围,返回异常功能码,寄存器值超出范围
					else
					{
						KeepRegister.DeviceAddress = dat;
						EEWrite(KREEPROM_BASEADDR,(void *)&dat,2);//保存数据
					}
					break;
		case 0x01:
					KeepRegister.DeviceMode = dat;
					EEWrite(KREEPROM_BASEADDR+2*0x01,(void *)&dat,2);//保存数据
					break;
		
		case 0x02:
					KeepRegister.IS_USER_MAG = dat;
					EEWrite(KREEPROM_BASEADDR+2*0x02,(void *)&dat,2);//保存数据
					break;
		
		case 0x09:
					KeepRegister.MAG_Mx = dat;
					EEWrite(KREEPROM_BASEADDR+2*0x09,(void *)&dat,2);//保存数据
					break;
		
		case 0x0a:
					KeepRegister.MAG_My = dat;
					EEWrite(KREEPROM_BASEADDR+2*0x0a,(void *)&dat,2);//保存数据
					break;
		
		case 0x0b:
					KeepRegister.MAG_Mz = dat;
					EEWrite(KREEPROM_BASEADDR+2*0x0b,(void *)&dat,2);//保存数据
					break;
		
		case 0x12:
					KeepRegister.DeviceLen = dat;
					InputRegister.Len = KeepRegister.DeviceLen;
					EEWrite(KREEPROM_BASEADDR+2*0x12,(void *)&dat,2);//保存数据
					break;
		default:
					err=err_OE;			
					break;
	}
	
	if(  err != 0 )			//返回异常码信息
	{
		ModbusReturnAckInfo(err);					//向485返回异常码信息
		return ERROR;
	}
	//指令返回
	
	temp[0] = KeepRegister.DeviceAddress;			//赋值设备地址
	temp[1] = ModbusDataPackage.dat[1];				//赋值功能码
	memcpy(&temp[2],(uint8_t *)&ModbusDataPackage.dat[2],4);
	//校验
	crc = CRC16_Check(temp,6);						//crc校验
	temp[6] = crc & 0xff;							//crc低位在前
	temp[7] = crc >> 8;								//高位在后
	if(ModbusDataPackage.dat[0]) U485SendData(temp,8);//发送数据
	
	return SUCCESS;
}
/********************************
写多个寄存器,保持寄存器
数据格式:器件地址(1字节)+功能码(0x10)+寄存器地址(2字节)+寄存器数量(2字节)+字节数(1字节)+寄存器数值(N个数据)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x10)+寄存器地址(2字节)+寄存器数量(2字节)+校验(2字节)
*********************************/
char ModbusWriteSomeRegistor(void)
{
	uint8_t err=0;
	uint8_t temp[10];
	//uint8_t t[180];
	uint16_t crc;
	//获取相关参数
	uint16_t StartAddress = (ModbusDataPackage.dat[2] << 8) | ModbusDataPackage.dat[3];	//获取起始地址
	uint16_t RegVal = (ModbusDataPackage.dat[4] << 8) | ModbusDataPackage.dat[5];		//获取要写入的寄存器数量
	uint8_t bytes = ModbusDataPackage.dat[6];											//获取字节数
	uint16_t StopAddress = StartAddress + RegVal - 1;									//获取结束地址
	uint16_t KeepRegistorSize = sizeof(KeepRegister) / 2;								//计算保持寄存器总数量
	
	//U485TX;Delay_ms(10);
	
	//printf("修正值:mm  修正后高度值:mm\r\n");
	//Delay_ms(100);U485RX;Delay_ms(10);
	
	//参数合法检查
	if(!RegVal || !bytes)err = err_add;													//寄存器地址不正确,读取数量必须大于1
	if( (  (StartAddress == 0x04) || (StartAddress == 0x06) || (StartAddress == 0x08) || \
				 (StartAddress == 0x0D) || (StartAddress == 0x0F) || (StartAddress == 0x11) || \
				 (StartAddress == 0x14) || (StartAddress == 0x16) || (StartAddress == 0x18) ) )err = err_add;//寄存器地址不正确,多字节数据要从头读出
 	if( (  (StopAddress == 0x03)  || (StopAddress == 0x05)  || (StopAddress == 0x07)  || \
				 (StopAddress == 0x0C)  || (StopAddress == 0x0E)  || (StopAddress == 0x10)  || \
	       (StopAddress == 0x13)  || (StopAddress == 0x15)  || (StopAddress == 0x17)  ) )err = err_add;//寄存器地址不正确,多字节数据没有全部读出
	if(StopAddress > (KeepRegistorSize - 1))err = err_add;		//寄存器地址不正确
	if(StartAddress > (KeepRegistorSize - 1))err = err_add;		//寄存器地址不正确
	
	
	if(  err != 0 )							//返回异常码信息
	{
		ModbusReturnAckInfo(err);									//向485返回异常码信息
		return ERROR;
	}
	
	memcpy((uint8_t *)&KeepRegisterTemp,(uint8_t *)&KeepRegister,sizeof(KeepRegister));					//先将当前寄存器数据拷贝到缓存中
	memcpy((uint8_t *)&KeepRegisterTemp + StartAddress*2,(uint8_t *)&ModbusDataPackage.dat[7],bytes);	//然后将接收的数据拷贝到寄存器缓存中
	err = 0;
	//大小端转换
	if(StartAddress == 0x00)KeepRegisterTemp.DeviceAddress = htons(KeepRegisterTemp.DeviceAddress);														//将设备地址数据大小端转换
	if((StartAddress <= 0x01) && (StopAddress >= 0x01))KeepRegisterTemp.DeviceMode = htons(KeepRegisterTemp.DeviceMode );										//将保留数据1数据大小端转换
	if((StartAddress <= 0x02) && (StopAddress >= 0x02))KeepRegisterTemp.IS_USER_MAG = htons(KeepRegisterTemp.IS_USER_MAG );										//将保留数据1数据大小端转换
	if((StartAddress <= 0x03) && (StopAddress >= 0x04))KeepRegisterTemp.OriginalPitch = __ltobf(KeepRegisterTemp.OriginalPitch);				
	if((StartAddress <= 0x05) && (StopAddress >= 0x06))KeepRegisterTemp.OriginalRoll = __ltobf(KeepRegisterTemp.OriginalRoll );						
	if((StartAddress <= 0x07) && (StopAddress >= 0x08))KeepRegisterTemp.OriginalAzimuth = __ltobf(KeepRegisterTemp.OriginalAzimuth );	
	
	if((StartAddress <= 0x09) && (StopAddress >= 0x09))KeepRegisterTemp.MAG_Mx = htons(KeepRegisterTemp.MAG_Mx);				
	if((StartAddress <= 0x0a) && (StopAddress >= 0x0a))KeepRegisterTemp.MAG_My = htons(KeepRegisterTemp.MAG_My );						
	if((StartAddress <= 0x0b) && (StopAddress >= 0x0b))KeepRegisterTemp.MAG_Mz = htons(KeepRegisterTemp.MAG_Mz );	
	
	if((StartAddress <= 0x0c) && (StopAddress >= 0x0d))KeepRegisterTemp.MAG_X_OFFSET = __ltobf(KeepRegisterTemp.MAG_X_OFFSET );		
	if((StartAddress <= 0x0e) && (StopAddress >= 0x0f))KeepRegisterTemp.MAG_Y_OFFSET = __ltobf(KeepRegisterTemp.MAG_Y_OFFSET );				
	if((StartAddress <= 0x10) && (StopAddress >= 0x11))KeepRegisterTemp.MAG_Z_OFFSET = __ltobf(KeepRegisterTemp.MAG_Z_OFFSET );
	
	
	if((StartAddress <= 0x12) && (StopAddress >= 0x12))KeepRegisterTemp.DeviceLen = htons(KeepRegisterTemp.DeviceLen );										//将保留数据1数据大小端转换

	if((StartAddress <= 0x14) && (StopAddress >= 0x13))KeepRegisterTemp.OriginalX = __ltobf(KeepRegisterTemp.OriginalX );
	if((StartAddress <= 0x16) && (StopAddress >= 0x15))KeepRegisterTemp.OriginalY = __ltobf(KeepRegisterTemp.OriginalY );				
	if((StartAddress <= 0x18) && (StopAddress >= 0x17))KeepRegisterTemp.OriginalZ = __ltobf(KeepRegisterTemp.OriginalZ );
	
	
	//判断数据有效性
	if((KeepRegisterTemp.DeviceAddress == 0) || (KeepRegisterTemp.DeviceAddress > 247))err = err_Re_VOR;	//地址数据超出范围,返回异常功能码,寄存器值超出范围

	if(!err)																						//如果无错误,则将缓存的数据拷贝到寄存器中
	{
		memcpy((uint8_t *)&KeepRegister,(uint8_t *)&KeepRegisterTemp,sizeof(KeepRegister));
		EEWrite(KREEPROM_BASEADDR,(void *)&KeepRegisterTemp,sizeof(KeepRegister));					//保存数据，更新整个寄存器组
		if((StartAddress <= 0x12) && (StopAddress >= 0x12))InputRegister.Len = KeepRegister.DeviceLen;
	}
	else if(ModbusDataPackage.dat[0])//如果数据范围错误
	{
		ModbusReturnAckInfo(err);	 //向485返回异常码信息
		return ERROR;
	}
	
	//指令返回
	temp[0] = KeepRegister.DeviceAddress;					//赋值设备地址
	temp[1] = ModbusDataPackage.dat[1];						//赋值功能码
	memcpy(&temp[2],(uint8_t *)&ModbusDataPackage.dat[2],4);//赋值寄存器地址和数量,共4字节
	//校验
	crc = CRC16_Check(temp,6);								//crc校验
	temp[6] = crc & 0xff;									//crc低位在前
	temp[7] = crc >> 8;										//高位在后
	if(ModbusDataPackage.dat[0]) U485SendData(temp,8);		//发送数据
	
	return SUCCESS;
}

/********************************
设定初值，完成后写保持寄存器
数据格式:器件地址(1字节)+功能码(0x41)+操作码(4字节)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x41)+所修改寄存器首地址(2字节)+所修改寄存器数量(2字节)+校验(2字节)
操作码:0x44,0x66,0x88,0xaa
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
	
	KeepRegister.OriginalPitch = InputRegister.pitch;   //获取当前姿态角
	KeepRegister.OriginalRoll = InputRegister.roll;   
	KeepRegister.OriginalAzimuth = InputRegister.azimuth;    
	
	EEWrite(KREEPROM_BASEADDR+6,(void *)&KeepRegister.OriginalPitch,12);    //同时将数据保存在EEPROM
	
	EEWrite(KREEPROM_BASEADDR+38,(void *)&KeepRegister.OriginalX,12);    //同时将数据保存在EEPROM
	
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
磁力计椭圆修正函数
收到指令后将连续的若干条磁力计原始采样数据返回至上位机
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
	//请绕8字
	for(i=0;i<300;i++)
	{
		HMC5883L_SetMode();
		ClC_WatchDogTask();											//看门狗任务
		HMC5883L_Task(MAG_data);								//磁力计采样任务，获取磁力计数据
		Delay(50);
		ClC_WatchDogTask();											//看门狗任务
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
设定初值，完成后写保持寄存器
数据格式:器件地址(1字节)+功能码(0x41)+操作码(4字节)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x41)+所修改寄存器首地址(2字节)+所修改寄存器数量(2字节)+校验(2字节)
操作码:0x33,0x55,0x77,0x99
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
* Description    : 指令处理函数，根据接收指令不同执行相应的任务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InstructionTask(void)
{
	unsigned char ix;
	unsigned short crc;
	RS485_RX();
	if(ModbusDataPackage.DataFlag)		  //数据包已接收完成
	{ 	
		for(ix=0;ix<ModbusDataPackage.DataLen;ix++)
		{
			ModbusDataPackage.dat[ix] =USART1_GetChar();//将串口数据放到指定buf
		}
		crc = CRC16_Check((uint8_t *)ModbusDataPackage.dat,ModbusDataPackage.DataLen-2 );
		if( (( crc == ( (ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1] << 8) |    ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2])        )) \
		||  ( ( (ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1]) == 0xff    ) && ((ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2]) == 0xff))  )				
		{
			WorkTime = 0;//清零计时时间
			if(ModbusDataPackage.dat[0] == KeepRegister.DeviceAddress)//是本机指令
			{
				switch(ModbusDataPackage.dat[1])
				{
					case ReadKeepRegistor:			//读保持寄存器
								ModbusReadKeepRegistor();
								break;
					case ReadInputRegistor:			//读输入寄存器
								ModbusReadInputRegistor();
								InputRegister.SystemWorkStatus=0x02;
								break;
					case WriteSingleRegistor:		//写单个保持寄存器
								ModbusWriteSingleRegistor();
								InputRegister.SystemWorkStatus=0x03;
								break;
					case SoftWare_Reset:
								ResetSoftware();
								break;
					case WriteSomeRegistor:			//写多个保持寄存器
								ModbusWriteSomeRegistor();
								InputRegister.SystemWorkStatus=0x04;
								break;
					case HMC5883SetOffset:		//5883校准
								HMC5883_Offset();
								InputRegister.SystemWorkStatus=0x05;
								break;
					case SetOriginalVal:		//设定初始值
								ModbusSetInitalValue();
								InputRegister.SystemWorkStatus=0x09;
								break;
					//case StartSampling:
								//ModbusStartSampling();
								//InputRegister.SystemWorkStatus=0x0a;
								//break;
					default:
								ModbusReturnAckInfo(err_Fu);//向485返回异常码信息,功能号无效
								break;
				}
			}
			/********************************
			广播指令，不支持恢复地址，一旦误操作，整条线路无法处理。。。
			广播指令，不回复，，，，
			数据格式:器件地址(1字节)+功能码(0x)+操作码(4字节)+校验(2字节) 
			字段:0x33,0x55,0x77,0x99
			*********************************/
			else if (ModbusDataPackage.dat[0] == 0x00)//是广播指令
			{

				switch(ModbusDataPackage.dat[1])
				{
					//case SetOriginalVal:		//设定初始值
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
		USART1_ClearBuf_Flag();				 //清空串口接收缓存
		ModbusDataPackage.DataLen = 0;  //先清空长度，注意清空顺序
		ModbusDataPackage.DataFlag = 0; //清空标记位
	}
}
/*******************************************************************************
* Function Name  : Euler_Solution
* Description    : 姿态解算函数，欧拉旋转矩阵方式
* Input          : 设备长度len，长度信息构成设备初始状态向量[0;0;-len]
* Input					 : pitch角、roll角、yaw角
* Input				   : 数据输出指针
* Output         : X,Y,Z值
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
	
	p=pitch * PI / 180;																		//转换为弧度
	r=roll  * PI / 180;																		//转换为弧度
	y=yaw   * PI / 180;																		//转换为弧度
	
	/* 计算好三个角度的正弦、余弦值，提高运算效率 */
	sp=sin(p);
	cp=cos(p);
	sr=sin(r);
	cr=cos(r);
	sy=sin(y);
	cy=cos(y);
	
	/* 计算旋转矩阵M中各元素数据 */
	m11 = cy*cp;
	m12 = -sy*cr + cy*sp*sr;
	m13 = sy*sr + cy*sp*cr;
	m21 = sy*cp;
	m22 = cy*cr + sy*sp*sr;
	m23 = -cy*sr + sy*sp*cr;
	m31 = 0;
	m32 = 0;
	m33 = cp*cr;
	
	/* 初始向量A=[0;0;len],变换后的姿态坐标=M·A */
	*pr     = 10   * m13 * a3;
	*(pr+1) = 10   * m23 * a3;
	*(pr+2) = 0-10 * m33 * a3;
	//log_info("X:%f,Y:%f,Z:%f\r\n",*pr ,*(pr+1),*(pr+2));
}

/*************************************************************
* * ProcessTask()任务处理函数
* *
* *
* *采用NED坐标系
*************************************************************/
unsigned char ProcessTask(void)
{
	//计算将采用NED坐标系（北、东、下）
	unsigned char i=0;
	float ACC_data[4]={0};
	float MAG_data[3]={0};
	float accx=0,accy=0,accz=0,acct=0;
	float mag_xh=0,mag_yh=0;
	double ACC_pitch=0,ACC_roll=0;
	unsigned short accCount=20;//定义加速度计采样组数，20组共1000个数据
	//unsigned int tt1=0;
	//unsigned char temp[180]={0},bytes=0;
	float XYZ[3]={0};
	//uint16_t crc;

	//Sampling_tryagain:
	/*********************加速度计采样任务*****************************************/
	//tt1=SysTick_Count1;
	for(i=0;i<accCount;i++)                 //将连续的1000个数据分accCount次采集，避免单一任务长时间执行
	{
		ClC_WatchDogTask();										//看门狗喂狗任务，看门狗复位间隔为1.6S
		InstructionTask();										//指令接收任务
		ADXL355_Samp50_Task(ACC_data);				//加速度采样任务，采集50个数据，排序后取中间10个求平均，返回平均数
		accx+=ACC_data[0];
		accy+=ACC_data[1];
		accz+=ACC_data[2];
		acct+=ACC_data[3];
		//log_info("accx:%f,accy:%f,accz:%f,acct:%f\r\n",ACC_data[0],ACC_data[1],ACC_data[2],ACC_data[3]);
	}
	
	//加速计坐标系变换
	IMU_Data.XACC=-accz/accCount;									//计算加速度均值
	IMU_Data.YACC=-accx/accCount;
	IMU_Data.ZACC=-accy/accCount;
	IMU_Data.TACC= acct/accCount;      						//计算温度数据
	
	InputRegister.XACC =  IMU_Data.XACC;
	InputRegister.YACC =  IMU_Data.YACC;
	InputRegister.ZACC =  IMU_Data.ZACC;
	InputRegister.TACC =  IMU_Data.TACC;
	
	/************************计算俯仰角和翻滚角**********************/
	ACC_roll  = atan2(IMU_Data.YACC, IMU_Data.ZACC); //计算翻滚角,弧度
	ACC_pitch = atan2(-IMU_Data.XACC, (IMU_Data.YACC*sin(ACC_roll) + IMU_Data.ZACC*cos(ACC_roll)) );//计算俯仰角，弧度
	
	InputRegister.pitch=ACC_pitch*180.0/PI;//计算俯仰角，转换为角度赋值到输入寄存器
	InputRegister.roll=ACC_roll*180.0/PI;	 //计算横滚角，转换为角度赋值到输入寄存器
	
	/*********************磁力计采样任务*****************************************/
	if(KeepRegister.IS_USER_MAG) 					//使用磁力计数据,则进行磁力计采样
	{
		//	do
		//	{
		//		Modbus_BusyFlag=0;		//侦听模式，使用串口中断功能监测485总线状态，如果有数据则舍弃数据
		//	}while(Modbus_FreeFlag);//先判断串口状态，如果无数据，继续，如果有数据，等待串口接收完成	
		//	Delay(5);	
		
		HMC5883L_MAG_TASK(MAG_data);
		
		//	Delay(5);
		//	if(Modbus_BusyFlag)
		//	{
		//		//log_info("Sampling Error!\r\n");
		//		return 0;
		//	} //goto Sampling_tryagain;			//侦听模式，使用串口中断功能监测485总线状态，如果有数据则舍弃数据
		
		//磁力计坐标系变换
		IMU_Data.XMAG = -MAG_data[2];
		IMU_Data.YMAG =  MAG_data[1];
		IMU_Data.ZMAG =  MAG_data[0];
	
		InputRegister.XMAG =  IMU_Data.XMAG;
		InputRegister.YMAG =  IMU_Data.YMAG;
		InputRegister.ZMAG =  IMU_Data.ZMAG;
		
		/******************航向角-倾斜修正*********************/
		//倾斜修正算法
		mag_xh = 	 InputRegister.XMAG  * cos(ACC_pitch) + InputRegister.YMAG * sin(ACC_pitch) * sin(ACC_roll) + InputRegister.ZMAG * cos(ACC_roll) * sin(ACC_pitch);
		mag_yh =  -InputRegister.YMAG  * cos(ACC_roll)  + InputRegister.ZMAG * sin(ACC_roll);

		//航向角计算
		IMU_Data.azimuth=atan2(mag_yh, mag_xh)*180.0/PI;
		if(IMU_Data.azimuth<0)IMU_Data.azimuth=IMU_Data.azimuth+360;
		InputRegister.azimuth=IMU_Data.azimuth;
	}
	else InputRegister.azimuth=0;	     //不使用磁力计数据,航向角直接赋为0

	/*   采用欧拉角进行姿态解算  */
	Euler_Solution(InputRegister.Len,InputRegister.pitch,InputRegister.roll,InputRegister.azimuth,XYZ);//调用姿态解算函数处理数据
	
	InputRegister.X = XYZ[0];
	InputRegister.Y = XYZ[1];
	InputRegister.Z = XYZ[2];
	
	InputRegister.OriginalX = KeepRegister.OriginalX;						//将保持寄存器中的原始XYZ数据赋值给输入寄存器
	InputRegister.OriginalY = KeepRegister.OriginalY;
	InputRegister.OriginalZ = KeepRegister.OriginalZ;
	
	InputRegister.OriginalPitch = KeepRegister.OriginalPitch;		//将保持寄存器中的原始角度数据赋值给输入寄存器
	InputRegister.OriginalRoll = KeepRegister.OriginalRoll;
	InputRegister.OriginalAzimuth = KeepRegister.OriginalAzimuth;
	
//	InputRegistorDataHton();									//大小端数据处理,放在缓存中
//	//读取输入寄存器数据并向485返回数据
//	bytes=sizeof(InputRegister);
//	//log_info("bytes:%d\r\n",bytes);
//	temp[0] = KeepRegister.DeviceAddress;						//赋值设备地址
//	temp[1] = 06;							//赋值功能码
//	temp[2] = bytes;											//赋值数据长度(字节数)
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
获取SN信息
数据格式:器件地址(1字节)+功能码(0x40)+操作码(4字节)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x40)+数据数量(1字节)+SN信息(15字节)+校验(2字节)
操作码:0x33,0x55,0x77,0x99
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
//	if(  (err != 0)  && (ModbusDataPackage.dat[0] != 0) )//加判断
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
	
	EERead_Z(EEPROM_SHARE_DATA_ADDR,SN_Info,16);//读取EEPROM中共享字段中的SN信息
	
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
恢复出厂设置
数据格式:器件地址(0)+功能码(0x68)+操作码(4字节)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x68)+寄存器地址(2字节)+寄存器数量(2字节)+字节数(1字节)+所有保持寄存器数据(N字节)+校验(2字节)
操作码:0x33,0x55,0x77,0x99
*********************************/
/*
char ModbusFactoryParameterReset(void)
{
	uint8_t err=0;
	uint8_t temp[255];
	uint16_t crc;
	if(ModbusDataPackage.DataLen != 8)err = err_OE;//有效操作发生异常
	if(strncmp(FactoryResetWord,(char *)&ModbusDataPackage.dat[2],4) != 0)err = err_OE;//寄存器值超出范围
	if(  (err != 0)  && (ModbusDataPackage.dat[0] != 0) )	  //返回异常码信息
	{
		ModbusReturnAckInfo(err);			  //向485返回异常码信息
		return ERROR;
	}
	
		
	KeepRegistorDataHton();				 //大小端数据处理,放在缓存中
										 
	temp[0] = KeepRegister.DeviceAddress;//赋值设备地址
	temp[1] = ModbusDataPackage.dat[1];	 //赋值功能码
	temp[2] = 0x00;
	temp[3] = 0x00;
	temp[4] = 0x00;
	temp[5] = KeepRegister_Num;          
	temp[6] = KeepRegister_Byte_Num;
	
	memcpy(&temp[7],(uint8_t *)&KeepRegisterTemp,KeepRegister_Byte_Num);//获取数据
	
	//校验
	crc = CRC16_Check(temp,KeepRegister_Byte_Num+7); //crc校验
	temp[KeepRegister_Byte_Num+7] = crc & 0xff;		//crc低位在前
	temp[KeepRegister_Byte_Num+8] = crc >> 8;		//高位在后
	
	if(ModbusDataPackage.dat[0]) U485SendData(temp,KeepRegister_Byte_Num+9);//发送数据,长度=读取字节数+前面三个字节+两个校验；如果是广播命令则不回复。
		
	EEErase(FPOWERONFLAG_BASEADDR,2);
	//EERead(FPOWERONFLAG_BASEADDR,temp,2);
	
	SystemResetSoft();
	
	return SUCCESS;
}
*/















