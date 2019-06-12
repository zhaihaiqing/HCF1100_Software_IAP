
#include "main.h"


/*******************************************************************************
* Function Name  : SPI2_Configuration
* Description    : STM32 SPI2配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI2_Configuration(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);													//打开外设时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB , ENABLE);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);                     //设置外设功能
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 |GPIO_Pin_14 | GPIO_Pin_15;				//设置端口模式，PA13=SCK2,PA14=MISO2,PA15=MOSI2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                                //外设复用模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;															//推挽模式
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;															//默认下拉
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;														//GPIO翻转速度为40MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);																			//将配置写入对应的GPIO寄存器中

	//ADXL355_CS
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8|GPIO_Pin_9;												//设置SPI片选，PA8=SCA3300_CS，PA9=ADXL355
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;																//端口输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;															//推挽模式
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;																//默认上拉，禁止AD片选
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;														//GPIO翻转速度为2MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);																			//将配置写入对应的GPIO寄存器中 
	GPIO_SetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_9);

	//ADXL355_DRDY
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  

	/* SPI2 设置 */
	SPI_Cmd(SPI2, DISABLE);
	SPI_I2S_DeInit(SPI2);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   	//SPI1设置为两线全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;									       	//设置SPI1为主模式
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;											//SPI发送接收8位帧结构
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;														//串行时钟空闲 低电平
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;													//第1个时钟沿开始采样数据
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;															//NSS信号由软件（使用SSI位）管理
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;		//定义波特率预分频的值:波特率预分频值为2
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;										//数据传输从MSB位开始
  SPI_InitStructure.SPI_CRCPolynomial = 7;															//CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);
  SPI_Cmd(SPI2, ENABLE); 
}

/*******************************************************************************
* Function Name  : ADS1248_ReadByte
* Description    : STM32 从SPI总线上读一个字节数据
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char SPI2_ReadByte(void)
{
	return (SPI2_WriteByte(0xff));
}

/*******************************************************************************
* Function Name  : SPI2_WriteByte
* Description    : STM32 向SPI1总线写一个字节数据
* Input          : 数据，无符号char型
* Output         : None
* Return         : 16位无符号整型
*******************************************************************************/
unsigned char SPI2_WriteByte(unsigned char data)
{
	
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);	// Loop while DR register in not emplty 
	
	SPI_I2S_SendData(SPI2, data);																	// Send u8 through the SPI bus peripheral 
	
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);// Wait to receive a u8 
	
	return SPI_I2S_ReceiveData(SPI2);															// Return the u8 read from the SPI bus 
}


/*******************************************************************************
* Function Name  : ADS1248_ReadREG
* Description    : STM32 通过SPI1读ADS1248寄存器值
* Input          : 地址
* Output         : None
* Return         : 寄存器值
*******************************************************************************/
unsigned char ADXL355_ReadREG(unsigned char Reg_addr)
{
	unsigned char Reg_addr_data;
	ADXL355_CS_L();
	__nop();__nop();
	SPI2_WriteByte((Reg_addr<<1) | READ);
	Reg_addr_data=SPI2_ReadByte();
	__nop();__nop();
	ADXL355_CS_H();
	return Reg_addr_data;
}

/*******************************************************************************
* Function Name  : ADS1248_WriteReg
* Description    : STM32 通过SPI1总线向ADS1248对应寄存器中写数据
* Input          : 寄存器地址及数据
* Output         : None
* Return         : None
*******************************************************************************/
void ADXL355_WriteReg(unsigned char Reg_addr,unsigned char data)
{
	ADXL355_CS_L();
	__nop();__nop();
	SPI2_WriteByte((Reg_addr<<1) | WRITE);
	SPI2_WriteByte(data);
	__nop();__nop();
	ADXL355_CS_H();
}

/*******************************************************************************
* Function Name  : ADXL355_Init
* Description    : ADXL355初始化函数
* Input          : 地址
* Output         : None
* Return         : 寄存器值
*******************************************************************************/
void ADXL355_Init(void)
{		
	SPI2_Configuration();
	
	Delay(5);
	ADXL355_WriteReg(ADXL355_Reset, 0x52);	//复位器件
	Delay(5);
	ADXL355_WriteReg(ADXL355_Filter, 0x03);	//关闭高通滤波器，开启低通滤波器，并选择ODR
	ADXL355_WriteReg(ADXL355_INT_MAP, 0x01);//选择RDY引脚输出数据准备好信号
	ADXL355_WriteReg(ADXL355_Range, 0xC1);	//选择I2C高速模式，中断引脚极性为1，量程范围±2g
	ADXL355_WriteReg(ADXL355_POWER_CTL, 0);	//选择测量模式
	Delay(300);
}

/*******************************************************************************
* Function Name  : ADXL355_Samp50_Task
* Description    : ADXL355采样子函数，执行50次采样，使用中值+平均数方式滤波
* Input          : 返回数据指针
* Output         : X轴、Y轴、Z轴以及温度数据
* Return         : None
*******************************************************************************/
void ADXL355_Samp50_Task(float * ACC_data)
{
	unsigned int i,j,data_NUM;
	unsigned char data=0;
	unsigned short temp;
	float tempVal;
	unsigned int Timeout;
	int x, y, z;
	
	float x_data[50]={0};
	float y_data[50]={0};
	float z_data[50]={0};
	float buff=0;
	
	float acc_x=0, acc_y=0, acc_z=0;
	
	data_NUM=50;

	//获取温度
	data = ADXL355_ReadREG(ADXL355_TEMP2);
	temp = data << 8;
	data = ADXL355_ReadREG(ADXL355_TEMP1);
	temp |= data;
	if(temp>=1852)
	{
		temp=temp-1852;
		tempVal=25-1.0*temp/9.05;
	}
	else
	{
		temp=1852-temp;
		tempVal=1.0*temp/9.05+25;
	}

	for(i=0;i<data_NUM;i++)
	{
			//__nop();__nop();//__nop();__nop();
			Timeout=0xffff;
			while( (!ADXL355_DRDY) && (Timeout--) );
			//while( (!(ADXL355_ReadREG(ADXL355_Status)&0x01)) && (Timeout--) );
			
			data = ADXL355_ReadREG(ADXL355_XDATA3);
			x = data << 24;
			data = ADXL355_ReadREG(ADXL355_XDATA2);
			x |= data << 16;
			data = ADXL355_ReadREG(ADXL355_XDATA1);
			x |= data << 8;
			x = x >> 12;
			
			data = ADXL355_ReadREG(ADXL355_YDATA3);
			y = data << 24;
			data = ADXL355_ReadREG(ADXL355_YDATA2);
			y |= data << 16;
			data = ADXL355_ReadREG(ADXL355_YDATA1);
			y |= data << 8;
			y = y >> 12;
			
			data = ADXL355_ReadREG(ADXL355_ZDATA3);
			z = data << 24;
			data = ADXL355_ReadREG(ADXL355_ZDATA2);
			z |= data << 16;
			data = ADXL355_ReadREG(ADXL355_ZDATA1);
			z |= data << 8;
			z = z >> 12;
			
			x_data[i]=x/262144.0;							//转换为加速度
			y_data[i]=y/262144.0;
			z_data[i]=z/262144.0;	
	}	
	//冒泡排序，采样50次，从小到大排序，取中间10个数，求均值
	for(i=0;i<data_NUM;i++)
	{
		for(j=i+1;j<data_NUM;j++)
		{
			if(x_data[i]>x_data[j])//从小到大，改为"<"变为从大到小,,,x轴
			{
				buff=x_data[i];
				x_data[i]=x_data[j];
				x_data[j]=buff;
			}
			if(y_data[i]>y_data[j])//从小到大，改为"<"变为从大到小,,,y轴
			{
				buff=y_data[i];
				y_data[i]=y_data[j];
				y_data[j]=buff;
			}
			if(z_data[i]>z_data[j])//从小到大，改为"<"变为从大到小,,,z轴
			{
				buff=z_data[i];
				z_data[i]=z_data[j];
				z_data[j]=buff;
			}
		}
	}

	for(i=data_NUM/2-4;i<data_NUM/2+6;i++)
	{
		acc_x+=x_data[i];
		acc_y+=y_data[i];
		acc_z+=z_data[i];
	}
//	ang_x=(atan2(acc_x,sqrt(acc_y*acc_y+acc_z*acc_z)))*180.0/3.1415926;      //转换为角度值
//	ang_y=(atan2(acc_y,sqrt(acc_x*acc_x+acc_z*acc_z)))*180.0/3.1415926;
//	ang_z=(atan2(acc_z,sqrt(acc_x*acc_x+acc_y*acc_y)))*180.0/3.1415926;
	
	ACC_data[0] = acc_x/10;
	ACC_data[1] = acc_y/10;
	ACC_data[2] = acc_z/10;
	ACC_data[3] = tempVal;

}


//void SCA3300PinInit(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
//	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB, ENABLE);
//	
//	//SPI 引脚配置
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_15;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
//	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
//	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//	
//	//SCA_CS
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	
//	GPIO_ResetBits(GPIOA, GPIO_Pin_8);
//}




//void SCA3300_Init(void)
//{
//	SCA3300PinInit();
//}

//uint8_t SCA3300_SPIWrite_ReadByteByte(uint8_t SPITxData)
//{
//	uint8_t SPICount,SPIRxData;                           // Counter used to clock out the data

//	SPIRxData = 0;
//	SPI2_CLK_L();
////	__nop();__nop();__nop();__nop();
////	__nop();__nop();__nop();__nop();		

//	for (SPICount = 0; SPICount < 8; SPICount++)
//	{	
//		SPIRxData <<= 1;
//		if (SPITxData & 0x80)
//		{
//			SPI2_MOSI_H();
//			
//		}
//		else
//		{
//			SPI2_MOSI_L();	
//		}
//		SPI2_CLK_H();
////		__nop();__nop();__nop();__nop();	
//		if(Read_SPI_MISO)
//		{
//		   SPIRxData|=0x01;
//		}
//		SPITxData <<= 1;
//		SPI2_CLK_L();   //Falling edge to collect data
//	} 

//	return (SPIRxData);                              // Finally return the read data
//}

//unsigned char SCA3300_ReadREG(unsigned char Reg_addr)
//{
//	unsigned char Reg_addr_data;

//	__nop();__nop();
//	SPI2_WriteByte(Reg_addr);
//	Reg_addr_data=SPI2_ReadByte();
//	__nop();__nop();

//	return Reg_addr_data;
//}

//uint32_t SCA3300GetData(uint32_t data)
//{
//	uint8_t recv_data[4], send_data;
//	uint32_t sca3300_id;
//	SCA3300_CS_L();
////	__nop();__nop();__nop();__nop();
////	__nop();__nop();__nop();__nop();		

//	send_data = (data >> 24) & 0xff;
//	
//	recv_data[0] = SCA3300_SPIWrite_ReadByteByte(send_data);
//	
//	send_data = (data >> 16) & 0xff;
//	recv_data[1] = SCA3300_SPIWrite_ReadByteByte(send_data);
//	
//	send_data = (data >> 8) & 0xff;
//	recv_data[2] = SCA3300_SPIWrite_ReadByteByte(send_data);
// 
//	send_data = data & 0xff;
//	
//	recv_data[3] = SCA3300_SPIWrite_ReadByteByte(send_data);	
//	
//	SCA3300_CS_H();
////	__nop();__nop();__nop();__nop();
////	__nop();__nop();__nop();__nop();		

//	sca3300_id = recv_data[0];
//	sca3300_id <<= 8;
//	sca3300_id |= recv_data[1];
//	sca3300_id <<= 8;
//	sca3300_id |= recv_data[2];
//	sca3300_id <<= 8;
//	sca3300_id |= recv_data[3];
//	
//	return (sca3300_id);	
//}

//void SCA3300_Sample(void)
//{
//	uint32_t status_sum;
//	int16_t tmp;
//	unsigned int x,y,z;
//	unsigned int temp,sto;
//	uint8_t  SCA3300_RSData;
//	float sensitivity;
//	double acc_x,acc_y,acc_z;
//	double ang_x,ang_y,ang_z;
//	float tempVal;
//	
//	SCA3300_Init();
//	Delay(10);
//	SCA3300GetData(SW_RESET);	
//	SCA3300GetData(SET_MODE_4);	//set mode
//	sensitivity= SENSITIVITY_4;
//	Delay(100);
//	
///************************* 读取数据 *************************/
//	SCA3300GetData(READ_ACC_X);    //请求X轴数据在下一个请求返回数据
//	SCA3300GetData(READ_ACC_X);    //请求X轴数据在下一个请求返回数据
//	x = SCA3300GetData(READ_ACC_Y);
//	y = SCA3300GetData(READ_ACC_Z);
//	z = SCA3300GetData(READ_STO);
//	sto = SCA3300GetData(READ_TEMP);
//	temp = SCA3300GetData(READ_TEMP);
//	
//	
//	tmp = ((x & DATA_FIELD_MASK)>>8);
//	acc_x = 1.0*tmp/sensitivity;
//	
//	tmp = ((y & DATA_FIELD_MASK)>>8);
//	acc_y = 1.0*tmp/sensitivity;
//	
//	tmp = ((z & DATA_FIELD_MASK)>>8);
//	acc_z = 1.0*tmp/sensitivity;
//	
//	tmp = (temp & DATA_FIELD_MASK)>>8;
//	tempVal = -273+(1.0*tmp/18.9);
//	
//	ang_x=(atan2(acc_x,sqrt(acc_y*acc_y+acc_z*acc_z)))*180.0/3.1415926;
//	ang_y=(atan2(acc_y,sqrt(acc_x*acc_x+acc_z*acc_z)))*180.0/3.1415926;
//	ang_z=(atan2(acc_z,sqrt(acc_x*acc_x+acc_y*acc_y)))*180.0/3.1415926;
//	
//	log_info("SCA3300:%f,%f,%f,%f\r\n",ang_x,-ang_y,ang_z,tempVal);
//	
//	

/////************************* 数据处理 *************************/
////	//STO数据有效性判断
////	SCA3300_RSData = (sca3300_Response_data.sto & RS_FIELD_MASK) >> 24;
//////	if((SCA3300_RSData != 1) || \
//////		(sca3300_Response_data.sto & 0xff) != CalculateCRC(sca3300_Response_data.sto | 0xffffff00))
////	if(SCA3300_RSData != 1)
////	{
////		sca3300_data.status = 1;
////		printf("ERR_RS_STO!\n");
////		goto ERR_SCA3300;
////	}	
////	//计算STO值
////	tmp = ((sca3300_Response_data.sto & DATA_FIELD_MASK)>>8);
////	if((tmp<-400) || (tmp>400))
////	{
////		sca3300_data.status = 2;
////		printf("ERR_STO![%d]\n", tmp);
////		goto ERR_SCA3300;
////	}
////	
////	//X数据有效性判断
////	SCA3300_RSData = (sca3300_Response_data.x & RS_FIELD_MASK) >> 24;
//////	if((SCA3300_RSData != 1) || \
//////		(sca3300_Response_data.x & 0xff) != CalculateCRC(sca3300_Response_data.x | 0xffffff00))
////	if(SCA3300_RSData != 1)
////	{
////		sca3300_data.status = 3;
////		printf("ERR_RS_ACCX!\n");
////		goto ERR_SCA3300;
////	}	
////	//X计算加速度值
////	tmp = ((sca3300_Response_data.x & DATA_FIELD_MASK)>>8);
//////	if(tmp & 0x8000)
//////	{
//////		tmp ^= 0xffff;
//////		++tmp;
//////		sca3300_data.ACC_X = -(tmp/MODELBS);	
//////	}
//////	else
//////	{
////		sca3300_data.ACC_X = tmp/sensitivity;
//////	}
////	
////	//Y数据有效性判断
////	SCA3300_RSData = (sca3300_Response_data.y & RS_FIELD_MASK)>>24;
//////	if((SCA3300_RSData != 1) ||	\
//////		(sca3300_Response_data.y & 0xff) != CalculateCRC(sca3300_Response_data.y | 0xffffff00))
////	if(SCA3300_RSData != 1)
////	{
////		sca3300_data.status = 4;
////		printf("ERR_RS_ACCY!\n");
////		goto ERR_SCA3300;
////	}
////	//Y计算加速度值
////	tmp = (sca3300_Response_data.y & DATA_FIELD_MASK)>>8;
//////	if(tmp & 0x8000)
//////	{
//////		tmp ^= 0xffff;
//////		++tmp;
//////		sca3300_data.ACC_Y = -(tmp/MODELBS);
//////	}
//////	else
//////	{
////		sca3300_data.ACC_Y = tmp/sensitivity;
//////	}
////	
////	//Z数据有效性判断
////	SCA3300_RSData = (sca3300_Response_data.z & RS_FIELD_MASK)>>24;
//////	if((SCA3300_RSData != 1) ||	\
//////		(sca3300_Response_data.z & 0xff) != CalculateCRC(sca3300_Response_data.z | 0xffffff00))
////	if(SCA3300_RSData != 1)
////	{
////		sca3300_data.status = 5;
////		printf("ERR_RS_ACCZ!\n");
////		goto ERR_SCA3300;
////	}
////	//Z计算加速度值
////	tmp = (sca3300_Response_data.z & DATA_FIELD_MASK)>>8;
//////	if(tmp & 0x8000)
//////	{
//////		tmp ^= 0xffff;
//////		++tmp;
//////		sca3300_data.ACC_Z = -(tmp/MODELBS);
//////	}
//////	else
//////	{
////		sca3300_data.ACC_Z = tmp/sensitivity;
//////	}
////	
////	//TEMP数据有效性判断
////	SCA3300_RSData = (sca3300_Response_data.temp & RS_FIELD_MASK)>>24;
//////	if((SCA3300_RSData != 1) ||	\
//////		(sca3300_Response_data.temp & 0xff) != CalculateCRC(sca3300_Response_data.temp | 0xffffff00))
////	if(SCA3300_RSData != 1)
////	{
////		sca3300_data.status = 6;
////		printf("ERR_RS_TEMP!\n");
////		goto ERR_SCA3300;
////	}
////	//计算TEMP值
////	tmp = (sca3300_Response_data.temp & DATA_FIELD_MASK)>>8;
////	sca3300_data.temp = -273+(tmp/18.9);
////	
////	sca3300_data.status = 0;
////	
////	return sca3300_data;

/////************************* 异常处理 *************************/	
////ERR_SCA3300:
//////		SCA3300GetData(SW_RESET);	
//////		OSTimeDlyHMSM(0, 0, 0, 20, OS_OPT_TIME_HMSM_STRICT, &err);
//////		SCA3300GetData(SETMODE);	//set mode3
//////		OSTimeDlyHMSM(0, 0, 0, 10, OS_OPT_TIME_HMSM_STRICT, &err);	
////		SCA3300GetData(READ_STATUS_SUM);
////		status_sum = SCA3300GetData(READ_STATUS_SUM);
////		tmp = ((status_sum & DATA_FIELD_MASK)>>8);
////	
////		input_reg[WORK_STATUS*SINGLE_REG_SIZE] &= (uint8_t)(~0x03);
////		input_reg[WORK_STATUS*SINGLE_REG_SIZE] |= (uint8_t)((tmp>>8)&0x03);
////		input_reg[WORK_STATUS*SINGLE_REG_SIZE+1] = (uint8_t)(tmp);
////		printf("status_sum = %X\n", tmp);
////	
////		SubSensor_PowerOff();
////		MainInit();		
////		SCA3300GetData(READ_STATUS_SUM);
////		SCA3300GetData(READ_STATUS_SUM);
////		printf("【ACC ERR!】\n");
//	
//		//return 0;		
//}

























