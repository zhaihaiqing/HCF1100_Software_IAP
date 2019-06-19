
#include "main.h"

#define ClockSpeed            100000   //速率400K

#define I2C_FLAG_TimeOut  		0xffff     //超时常量 0x5000
#define I2C_LONG_TimeOut  		(10 * I2C_FLAG_TimeOut)

 __IO uint32_t  HMC5883L_Timeout = HMC5883L_FLAG_TIMEOUT;

void I2C1_Init(void)
{
  I2C_InitTypeDef I2C_InitStructure;
	
				//定义GPIO结构体，定义I2C结构体
  GPIO_InitTypeDef GPIO_InitStructure;
  
  /* Enable I2C1 和GPIO clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* Reset I2C1 peripheral */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1,  ENABLE);
	/* Release reset signal of I2C1 IP */
  RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
	
	//将外设连接到对应端口上
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
	
  /* Configure I2C1 pins: SCL and SDA --GPIO8 and GPIO9 */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//外设复用模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//开漏输出
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;//无上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//外设复用模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//开漏输出
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;//无上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_DeInit(I2C1);
    //I2C_SoftwareResetCmd(I2C1, ENABLE);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed =  ClockSpeed;

    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_ITConfig(I2C1, I2C_IT_ERR, ENABLE);
    I2C_Cmd(I2C1, ENABLE);
}

unsigned char HMC5883L_CheckID(void)
{          
    uint8_t tmp_data;
	
    HMC5883L_I2CRead(HMC5883L_IRA, &tmp_data, sizeof(tmp_data));
	
    return tmp_data;
}

unsigned char HMC5883L_I2CRead( unsigned char reg_name, unsigned char* read_ptr,  unsigned char len)
{
	uint8_t err;

    HMC5883L_Timeout = HMC5883L_LONG_TIMEOUT;

    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))//EV5
    {
        if((HMC5883L_Timeout--) == 0)
        {
			err = 1;
            goto HMC5883L_I2C_READ_ERR;
        }
    }
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    I2C_GenerateSTART(I2C1, ENABLE);
    
    HMC5883L_Timeout = HMC5883L_FLAG_TIMEOUT;
    while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB))
    {
        if((HMC5883L_Timeout--) == 0)
        {
			err = 2;
            goto HMC5883L_I2C_READ_ERR;
        }
    }
	
    I2C_Send7bitAddress(I2C1, HMC5883L_ADDR, I2C_Direction_Transmitter);
    HMC5883L_Timeout = HMC5883L_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if((HMC5883L_Timeout--) == 0)
        {
			err = 3;
            goto HMC5883L_I2C_READ_ERR;
        }
    }
    I2C_SendData(I2C1, reg_name);
    HMC5883L_Timeout = HMC5883L_FLAG_TIMEOUT;
    while ((!I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF)))
    {
        if((HMC5883L_Timeout--) == 0)
        {
			err = 4;
            goto HMC5883L_I2C_READ_ERR;
        }
    }
    I2C_GenerateSTART(I2C1, ENABLE);
    
    HMC5883L_Timeout = HMC5883L_FLAG_TIMEOUT;
    while(!(I2C_GetFlagStatus(I2C1, I2C_FLAG_SB)))
    {
        if((HMC5883L_Timeout--) == 0)
        {
			err = 5;
            goto HMC5883L_I2C_READ_ERR;
        }
    }
    I2C_Send7bitAddress(I2C1, HMC5883L_ADDR, I2C_Direction_Receiver);
    HMC5883L_Timeout = HMC5883L_FLAG_TIMEOUT;
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if((HMC5883L_Timeout--) == 0)
        {
			err = 6;
            goto HMC5883L_I2C_READ_ERR;
        }
    }

    while(len)
    {
        if(len==1)
        {
            I2C_AcknowledgeConfig(I2C1, DISABLE);
            //I2C_GenerateSTOP(I2C1, ENABLE);
        }
        HMC5883L_Timeout = HMC5883L_LONG_TIMEOUT;
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))
        {
            if((HMC5883L_Timeout--) == 0)
            {
				err = 7;
                goto HMC5883L_I2C_READ_ERR;
            }
        }
        *read_ptr = I2C_ReceiveData(I2C1);
        if(len==1)
        {
            //I2C_AcknowledgeConfig(I2C1, DISABLE);
            I2C_GenerateSTOP(I2C1, ENABLE);
        }
        ++read_ptr;
        --len;
    }
    return SUCCESS;
    
HMC5883L_I2C_READ_ERR:    
	return 0;
}

unsigned char HMC5883L_I2CWrite(unsigned char reg_name, unsigned char* write_ptr, unsigned char len)
{
	uint8_t err;
	
    HMC5883L_Timeout = HMC5883L_FLAG_TIMEOUT;
    I2C_AcknowledgeConfig(I2C1, ENABLE);
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {
        if((HMC5883L_Timeout--) == 0)
        {
			err = 1;
            goto HMC5883L_I2C_WRITE_ERR;
        }
    }
    //S Start
    I2C_GenerateSTART(I2C1, ENABLE);
    // EV5 
    while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB))
    {
        if((HMC5883L_Timeout--) == 0)
        {
			err = 2;
            goto HMC5883L_I2C_WRITE_ERR;
        }
    }
    I2C_Send7bitAddress(I2C1, HMC5883L_ADDR, I2C_Direction_Transmitter);
    //EV6
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if((HMC5883L_Timeout--) == 0)
        {
			err = 3;
            goto HMC5883L_I2C_WRITE_ERR;
        }
    }
    I2C_SendData(I2C1, reg_name);
    while ((!I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF)))
    {
        if((HMC5883L_Timeout--) == 0)
        {
			err = 4;
            goto HMC5883L_I2C_WRITE_ERR;
        }
    }
    while(len)
    {
        I2C_SendData(I2C1, *write_ptr);
        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        {
            if((HMC5883L_Timeout--) == 0)
            {
				err = 5;
                goto HMC5883L_I2C_WRITE_ERR;
            }
        }
        if(len==1)
        {
            I2C_GenerateSTOP(I2C1, ENABLE);
        }
        ++write_ptr;
        --len;
    }
    return SUCCESS;
	
HMC5883L_I2C_WRITE_ERR:		
	return 0;
}

//void Azimuth_Correct(incli_angle_data incli_angle_tmp)
//{
//	float Hx, Hy;
//	float yaw;
//	
//	Hx = incli_angle_tmp.hx * cos(incli_angle_tmp.pitch) 				\
//		+ incli_angle_tmp.hy * sin(incli_angle_tmp.pitch) * sin(incli_angle_tmp.roll)	\
//		+ incli_angle_tmp.hz * sin(incli_angle_tmp.pitch) * cos(incli_angle_tmp.roll);
//	
//	Hy = incli_angle_tmp.hy * cos(incli_angle_tmp.roll) + incli_angle_tmp.hz * sin(incli_angle_tmp.roll);
//	
//	
////	Yh = incli_angle_tmp.yh * cos(incli_angle_tmp.roll) 				\
////		+ incli_angle_tmp.xh * sin(incli_angle_tmp.roll) * sin(incli_angle_tmp.pitch)	\
////		- incli_angle_tmp.zh * cos(incli_angle_tmp.pitch) * sin(incli_angle_tmp.roll);
////	
////	Xh = incli_angle_tmp.xh * cos(incli_angle_tmp.pitch) + incli_angle_tmp.zh * sin(incli_angle_tmp.pitch);

//#if 0
////	if(Hx > 0)
////	{
////		if(Hy > 0)
////		{
////			yaw = 360 - atan(Hy/Hx)/3.1415926*180.0;
////		}
////		else if(Hy == 0)
////		{
////			yaw = 0;
////		}
////		else if(Hy < 0)
////		{
////			yaw = -atan(Hy/Hx)/3.1415926*180.0;
////		}
////	
////	}
////	else if(Hx == 0)
////	{
////		if(Hy > 0)
////		{
////			yaw = 270;
////		}
////		else if(Hy == 0)
////		{
////			yaw = -1;
////		//	yaw = atan2(Hy, Hx)/3.1415926*180.0;
////		}
////		else if(Hy < 0)
////		{
////			yaw = 90;
////		}
////	
////	}
////	else if(Hx < 0)
////	{
////		if(Hy > 0)
////		{
////		//
////			yaw = 180 - atan(Hy/Hx)/3.1415926*180.0;
////		}
////		else if(Hy == 0)
////		{
////			yaw = 180;
////		}
////		else if(Hy < 0)
////		{
////			yaw = 180 - atan(Hy/Hx)/3.1415926*180.0;
////		}
////	}
//	
//	//考虑到角度的4 个象限，航向角的计算公式可变为以下公式
//	if(Hx<0)
//	{
//		yaw =180 - (atan(Hy/Hx)*180/3.1415926);
//	}
//	else if(Hx>0 && Hy<0)
//	{
//		yaw = -(atan(Hy/Hx)*180/3.1415926);
//	}
//	else if(Hx>0 && Hy>0)
//	{
//		yaw = 360-(atan(Hy/Hx)*180/3.1415926);
//	}
//	else if(Hx==0 && Hy<0)
//	{
//		yaw =90;
//	}
//	else if(Hx==0 && Hy>0)
//	{
//		yaw=270;
//	}
//	
//#else
////	yaw = atan2(Hy, Hx)/3.1415926*180.0;
//	yaw = atan2(Hx, Hy)/3.1415926*180.0;
//	
//	yaw -= 90;
//	
//	if(yaw < 0)
//	{
//		yaw += 360;
//	}
//#endif
//	
////	printf("【sin30 = %f°】\n", sin(30*3.1415926/180.0));
//}


void HMC5883L_Init(void)
{
	//uint8_t tmp_data;
	//signed short MAG_data[3];
	I2C1_Init();
	//HMC5883L_ST(MAG_data);
	
}

void HMC5883L_SetMode(void)
{
	uint8_t tmp_data;
		
	tmp_data = HMC5883L_CRA_MA_8 | HMC5883L_CRA_DO_30Hz | HMC5883L_CRA_MS_Normal;
	HMC5883L_I2CWrite(HMC5883L_CRA, &tmp_data, 1);    //MA1=1,MA0=1,8个数据平均模式，DO2-0，data Rate，MS1-MS0，测量模式选择
	tmp_data = HMC5883L_CRB_GN_1_3;
	HMC5883L_I2CWrite(HMC5883L_CRB, &tmp_data, 1);
	tmp_data = HMC5883L_MODE_MD_CONTINUOUS;
	HMC5883L_I2CWrite(HMC5883L_MODE, &tmp_data, 1);
	//Delay(20);
}

unsigned char HMC5883L_Task(signed short int * px)
{
	uint8_t tmp_data;
	int16_t x_data=0, y_data=0, z_data=0;
	uint32_t Timeout=0x3fffff;
	
	while( HMC5883L_RDY && (Timeout--))
	{
		if(Timeout==0)return ERROR;
		//HMC5883L_I2CRead(HMC5883L_STATUS, &tmp_data, sizeof(tmp_data));
//		if(tmp_data & 0x01)
//		{
//			break;
//		}
	}
	
	HMC5883L_I2CRead(HMC5883L_X_M, &tmp_data, sizeof(tmp_data));
	x_data = tmp_data << 8;
	HMC5883L_I2CRead(HMC5883L_X_L, &tmp_data, sizeof(tmp_data));
	x_data |= tmp_data;
	
	HMC5883L_I2CRead(HMC5883L_Y_M, &tmp_data, sizeof(tmp_data));
	y_data = tmp_data << 8;
	HMC5883L_I2CRead(HMC5883L_Y_L, &tmp_data, sizeof(tmp_data));
	y_data |= tmp_data;
	
	HMC5883L_I2CRead(HMC5883L_Z_M, &tmp_data, sizeof(tmp_data));
	z_data = tmp_data << 8;
	HMC5883L_I2CRead(HMC5883L_Z_L, &tmp_data, sizeof(tmp_data));
	z_data |= tmp_data;

	*px=x_data;
	*(px+1)=y_data;
	*(px+2)=z_data;
	return SUCCESS;
}
/*******************************************************************************
* Function Name  : HMC5883L_MAG_TASK
* Description    : HMC5883采样子任务，采样30次，采用中值+平均滤波方法
* Input          : 数据返回指针
* Output         : X,Y,Z磁力计值
* Return         : None
*******************************************************************************/
void HMC5883L_MAG_TASK(float * px)
{
	uint8_t tmp_data=0;
	unsigned char Samp_Count=30;
	unsigned char i=0,j=0;
	signed short int MAG_out_data[3]={0};
	float mx=0,my=0,mz=0;
	float mtx=0,mty=0,mtz=0;
	float mttx[50]={0},mtty[50]={0},mttz[50]={0},buff;
	
	float MKx=0,MKy=0,MKz=0;
	
	//计算各轴长系数比
	MKx=1.0*KeepRegister.MAG_Mx/KeepRegister.MAG_Mx;
	MKy=1.0*KeepRegister.MAG_Mx/KeepRegister.MAG_My;
	MKz=1.0*KeepRegister.MAG_Mx/KeepRegister.MAG_Mz;
	
	HMC5883L_SetMode();	
	
	
	
	
	for(i=0;i<Samp_Count;i++)                   //将连续的magCount个数据分accCount次采集，避免单一任务长时间执行
	{
		InstructionTask();												//分组响应其他任务
		HMC5883L_Task(MAG_out_data);							//磁力计采样任务，获取磁力计数据
		ClC_WatchDogTask();											  //看门狗任务
		
		//航向角-椭圆修正，硬铁失真
		mx =  MKy  * (MAG_out_data[1] - KeepRegister.MAG_Y_OFFSET);
		my =  MKx  * (MAG_out_data[0] - KeepRegister.MAG_X_OFFSET);
		mz = -MKz  * (MAG_out_data[2] - KeepRegister.MAG_Z_OFFSET);
		
		mttx[i] = 130.0 * mx/2048;
		mtty[i] = 130.0 * my/2048;
		mttz[i] = 130.0 * mz/2048;
	}
//	for(i=0;i<Samp_Count;i++)
//	{
//		log_info("%d,mttx:%f,mtty:%f,mttz:%f\r\n",i,mttx[i],mtty[i],mttz[i]);
//	}

	//排序
		//冒泡排序，采样magCount次，从小到大排序，取中间5个数，求均值
	for(i=0;i<Samp_Count;i++)
	{
		for(j=i+1;j<Samp_Count;j++)
		{
			if(mttx[i]>mttx[j])//从小到大，改为"<"变为从大到小,,,x轴
			{
				buff=mttx[i];
				mttx[i]=mttx[j];
				mttx[j]=buff;
			}
			if(mtty[i]>mtty[j])//从小到大，改为"<"变为从大到小,,,x轴
			{
				buff=mtty[i];
				mtty[i]=mtty[j];
				mtty[j]=buff;
			}
			if(mttz[i]>mttz[j])//从小到大，改为"<"变为从大到小,,,x轴
			{
				buff=mttz[i];
				mttz[i]=mttz[j];
				mttz[j]=buff;
			}
		}
	}
	
	for(i=(Samp_Count/2-2);i<(Samp_Count/2+3);i++)
	{
		mtx+=mttx[i];
		mty+=mtty[i];
		mtz+=mttz[i];
	}
	
	*px=mtx/5;
	*(px+1)=mty/5;
	*(px+2)=mtz/5;
	
}

void HMC5883L_SelfTest(void)
{
	uint8_t tmp_data;
	signed short mxp,myp,mzp;
	signed short mxn,myn,mzn;
	
	tmp_data=0x40;
	HMC5883L_I2CWrite(HMC5883L_CRA, &tmp_data, 1);//设置增益

	tmp_data=0x11;
	HMC5883L_I2CWrite(HMC5883L_CRA, &tmp_data, 1);//设置CSRA
	tmp_data=0x01;
	HMC5883L_I2CWrite(HMC5883L_MODE, &tmp_data, 1);//设置模式
	Delay(50);
	
	HMC5883L_I2CRead(HMC5883L_X_M, &tmp_data, sizeof(tmp_data));
	mxp = tmp_data << 8;
	HMC5883L_I2CRead(HMC5883L_X_L, &tmp_data, sizeof(tmp_data));
	mxp |= tmp_data;
	
	HMC5883L_I2CRead(HMC5883L_Z_M, &tmp_data, sizeof(tmp_data));
	mzp = tmp_data << 8;
	HMC5883L_I2CRead(HMC5883L_Z_L, &tmp_data, sizeof(tmp_data));
	mzp |= tmp_data;
	
	HMC5883L_I2CRead(HMC5883L_Y_M, &tmp_data, sizeof(tmp_data));
	myp = tmp_data << 8;
	HMC5883L_I2CRead(HMC5883L_Y_L, &tmp_data, sizeof(tmp_data));
	myp |= tmp_data;
	
		//Write CONFIG_A register and do positive test
	tmp_data=0x12;
	HMC5883L_I2CWrite(HMC5883L_CRA, &tmp_data, 1);//设置CSRA
	tmp_data=0x01;
	HMC5883L_I2CWrite(HMC5883L_MODE, &tmp_data, 1);//设置模式
	Delay(50);
	
	HMC5883L_I2CRead(HMC5883L_X_M, &tmp_data, sizeof(tmp_data));
	mxn = tmp_data << 8;
	HMC5883L_I2CRead(HMC5883L_X_L, &tmp_data, sizeof(tmp_data));
	mxn |= tmp_data;
	
	HMC5883L_I2CRead(HMC5883L_Z_M, &tmp_data, sizeof(tmp_data));
	mzn = tmp_data << 8;
	HMC5883L_I2CRead(HMC5883L_Z_L, &tmp_data, sizeof(tmp_data));
	mzn |= tmp_data;
	
	HMC5883L_I2CRead(HMC5883L_Y_M, &tmp_data, sizeof(tmp_data));
	myn = tmp_data << 8;
	HMC5883L_I2CRead(HMC5883L_Y_L, &tmp_data, sizeof(tmp_data));
	myn |= tmp_data;

	
//	RS485_TX();
//	log_info("mxp:%d,myp:%d,mzp:%d,mxn:%d,myn:%d,mzn:%d\r\n",mxp,myp,mzp,mxn,myn,mzn);
//	RS485_RX();
	
	
	
	
	
}









