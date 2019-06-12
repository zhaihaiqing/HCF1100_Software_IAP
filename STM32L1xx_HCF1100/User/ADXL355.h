#ifndef __ADXL355_H
#define __ADXL355_H

/*******************   ADXL355�Ĵ����б�   ***************************************/
#define	ADXL355_DEVID_AD        0x00      //ģ������ID
#define	ADXL355_DEVID_MST       0x01		  //�ڲ�MEMS ID
#define	ADXL355_PARTID  				0x02		  //����ID
#define	ADXL355_REVID  					0x03			//Mask revision 
#define	ADXL355_Status  				0x04			//״̬�Ĵ���  [7:5]����;[4]NVM_BUSY;[3]Activity;[2]FIFO_OVR;[1]FIFO_FULL;[0]DATA_RDY;
#define	ADXL355_FIFO_ENTRIES  	0x05			//FIFOָʾ�Ĵ��� [7]������[6:0]FIFO_Entries
#define	ADXL355_TEMP2  					0x06			//�¶ȼĴ���2   [7:4]������[3:0]temp[11:8]
#define	ADXL355_TEMP1  					0x07			//�¶ȼĴ���1   temp[7:0]
#define	ADXL355_XDATA3  				0x08			//X�����ݼĴ���3
#define	ADXL355_XDATA2  				0x09			//X�����ݼĴ���2
#define	ADXL355_XDATA1  				0x0A			//X�����ݼĴ���1
#define	ADXL355_YDATA3  				0x0B			//Y�����ݼĴ���3
#define	ADXL355_YDATA2  				0x0C			//Y�����ݼĴ���2
#define	ADXL355_YDATA1  				0x0D			//Y�����ݼĴ���1
#define	ADXL355_ZDATA3  				0x0E			//Z�����ݼĴ���3
#define	ADXL355_ZDATA2  				0x0F			//Z�����ݼĴ���2
#define	ADXL355_ZDATA1  				0x10			//Z�����ݼĴ���1
#define	ADXL355_FIFO_DATA  			0x11			//FIFO���ʼĴ���
#define	ADXL355_OFFSET_X_H  		0x1E			//X��ƫ�ƼĴ���
#define	ADXL355_OFFSET_X_L  		0x1F			//X��ƫ�ƼĴ���
#define	ADXL355_OFFSET_Y_H  		0x20			//Y��ƫ�ƼĴ���
#define	ADXL355_OFFSET_Y_L  		0x21			//Y��ƫ�ƼĴ���
#define	ADXL355_OFFSET_Z_H  		0x22			//Z��ƫ�ƼĴ���
#define	ADXL355_OFFSET_Z_L  		0x23			//Z��ƫ�ƼĴ���
#define	ADXL355_ACT_EN  				0x24
#define	ADXL355_ACT_THRESH_H  	0x25
#define	ADXL355_ACT_THRESH_L  	0x26
#define	ADXL355_ACT_COUNT  			0x27
#define	ADXL355_Filter  				0x28			//�˲��Ĵ�����HPF,LPF
#define	ADXL355_FIFO_SAMPLES  	0x29			//FIFO�����Ĵ���
#define	ADXL355_INT_MAP  				0x2A			//Interrupt Pin ���ܼĴ���
#define	ADXL355_Sync  					0x2B			//����ͬ���Ĵ���
#define	ADXL355_Range  					0x2C			//���̼�ͨ�żĴ�����[7]I2C Speed��[6]Interrupt polarity��[5:2]������[1:0]���̣�01-��2g,10-��4g,11-��8g
#define	ADXL355_POWER_CTL  			0x2D			//��Դ���ƼĴ��� [7:3]������[2]DRDY_OFF��[1]TEMP_OFF��[0]STANDBY
#define	ADXL355_SELF_TEST  			0x2E			//�Լ�Ĵ���
#define	ADXL355_Reset 					0x2F			//��λ�Ĵ���������0x52��λ�����������ϵ縴λ




/************   SCA3300    ***********************/
//CMD
#define  READ_ACC_X				0x040000F7
#define  READ_ACC_Y   		0x080000FD
#define  READ_ACC_Z   		0x0C0000FB
#define  READ_STO					0x100000E9
#define  READ_TEMP				0x140000EF
#define  READ_STATUS_SUM 	0x180000E5
#define  SW_RESET					0xB4002098
#define  READ_WHO_AM_I		0x40002091

//Mode
#define  SET_MODE_1   0xB400001F
#define  SET_MODE_2   0xB4000102
#define  SET_MODE_3   0xB4000225
#define  SET_MODE_4   0xB4000338

#define SENSITIVITY_1 	2700.0
#define SENSITIVITY_2 	1350.0
#define SENSITIVITY_3 	5400.0
#define SENSITIVITY_4 	5400.0

// SPI frame field masks
#define OPCODE_FIELD_MASK       0xFC000000
#define RS_FIELD_MASK           0x03000000
#define DATA_FIELD_MASK         0x00FFFF00
#define CRC_FIELD_MASK          0x000000FF

enum{
	WRITE = 0,
	READ = 1
};

#define  SCA3300_CS_H()	 GPIO_SetBits(GPIOA, GPIO_Pin_8)
#define  SCA3300_CS_L()	 GPIO_ResetBits(GPIOA, GPIO_Pin_8)

#define  ADXL355_CS_H()	 GPIO_SetBits(GPIOA, GPIO_Pin_9)
#define  ADXL355_CS_L()	 GPIO_ResetBits(GPIOA, GPIO_Pin_9)

#define  SPI2_CLK_H()	 GPIO_SetBits(GPIOB, GPIO_Pin_13)
#define  SPI2_CLK_L()	 GPIO_ResetBits(GPIOB, GPIO_Pin_13)

#define  SPI2_MOSI_H()	 GPIO_SetBits(GPIOB, GPIO_Pin_15)
#define  SPI2_MOSI_L()	 GPIO_ResetBits(GPIOB, GPIO_Pin_15)

#define Read_SPI_MISO    GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)

#define ADXL355_DRDY    GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12)


void SPI2_Configuration(void);
unsigned char SPI2_ReadByte(void);
unsigned char SPI2_WriteByte(unsigned char data);
unsigned char ADXL355_ReadREG(unsigned char Reg_addr);
void ADXL355_WriteReg(unsigned char Reg_addr,unsigned char data);

void ADXL355_Init(void);
void ADXL355_Samp50_Task(float * ACC_data);




#endif


