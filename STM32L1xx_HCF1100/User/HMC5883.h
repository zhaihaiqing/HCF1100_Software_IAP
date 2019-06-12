#ifndef __HMC5883_H
#define __HMC5883_H

#define    HMC5883L_CRA    		0x00		//���üĴ���A
#define    HMC5883L_CRB    		0x01		//���üĴ���B
#define    HMC5883L_MODE    	0x02		//ģʽ�Ĵ���
#define    HMC5883L_X_M    		0x03		//���ݼĴ��� X
#define    HMC5883L_X_L    		0x04		//���ݼĴ��� X
#define    HMC5883L_Z_M    		0x05		//���ݼĴ��� Z
#define    HMC5883L_Z_L    		0x06		//���ݼĴ��� Z
#define    HMC5883L_Y_M    		0x07		//���ݼĴ��� Y
#define    HMC5883L_Y_L   		0x08		//���ݼĴ��� Y
#define    HMC5883L_STATUS    0x09		//״̬�Ĵ���
#define    HMC5883L_IRA    		0x0a		//ʶ��Ĵ���A
#define    HMC5883L_IRB    		0x0b		//ʶ��Ĵ���B
#define    HMC5883L_IRC    		0x0c		//ʶ��Ĵ���C

#define I2C1_SLAVE_ADDRESS7   0x30 //���������ַ
#define HMC5883L_ADDR    0x3c   //HMC5833�����ַ

#define HMC5883L_FLAG_TIMEOUT  ((uint32_t)0xf000)
#define HMC5883L_LONG_TIMEOUT  ((uint32_t)(HMC5883L_FLAG_TIMEOUT*10))

#define I2C_SCL_CLK			RCC_AHBPeriph_GPIOB
#define I2C_SCL_PORT		GPIOB
#define I2C_SCL_PIN			GPIO_Pin_8

#define I2C_SDA_CLK			RCC_AHBPeriph_GPIOB
#define I2C_SDA_PORT		GPIOB
#define I2C_SDA_PIN			GPIO_Pin_9

#define HMC_DADY_CLK		RCC_AHBPeriph_GPIOB
#define HMC_DADY_PORT		GPIOB
#define HMC_DADY_PIN		GPIO_Pin_3

#define HMC5883L_RDY    GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3)

#define HMC5883L_CRA_MA_1   						(0x00<<5)
#define HMC5883L_CRA_MA_2   						(0x01<<5)
#define HMC5883L_CRA_MA_4   						(0x02<<5)
#define HMC5883L_CRA_MA_8   						(0x03<<5)

#define HMC5883L_CRA_DO_0_75Hz   				(0x00<<2)
#define HMC5883L_CRA_DO_1_5Hz   				(0x01<<2)
#define HMC5883L_CRA_DO_3Hz   					(0x02<<2)
#define HMC5883L_CRA_DO_7_5Hz  	 				(0x03<<2)
#define HMC5883L_CRA_DO_15Hz 						(0x04<<2)
#define HMC5883L_CRA_DO_30Hz 						(0x05<<2)
#define HMC5883L_CRA_DO_75Hz 						(0x06<<2)
#define HMC5883L_CRA_DO_NU 							(0x07<<2)

#define HMC5883L_CRA_MS_Normal 					0x00
#define HMC5883L_CRA_MS_Positive 				0x01
#define HMC5883L_CRA_MS_Negative 				0x02
#define HMC5883L_CRA_MS_Reserved 				0x03

#define HMC5883L_CRB_GN_0_88 						(0x00<<5)
#define HMC5883L_CRB_GN_1_3 						(0x01<<5)
#define HMC5883L_CRB_GN_1_9 						(0x02<<5)
#define HMC5883L_CRB_GN_2_5 						(0x03<<5)
#define HMC5883L_CRB_GN_4 							(0x04<<5)
#define HMC5883L_CRB_GN_4_7 						(0x05<<5)
#define HMC5883L_CRB_GN_5_6 						(0x06<<5)
#define HMC5883L_CRB_GN_8_1 						(0x07<<5)

#define HMC5883L_MODE_MD_CONTINUOUS 		0x00
#define HMC5883L_MODE_MD_SINGLE 				0x01
#define HMC5883L_MODE_MD_IDLE 					0x02
#define HMC5883L_MODE_MD_IDLE1 					0x03


 
void I2C1_Init(void);
unsigned char HMC5883L_CheckID(void);
unsigned char HMC5883L_I2CRead( unsigned char reg_name, unsigned char* read_ptr,  unsigned char len);
unsigned char HMC5883L_I2CWrite(unsigned char reg_name, unsigned char* write_ptr, unsigned char len);
void HMC5883L_Init(void);
unsigned char HMC5883L_Task(signed short int * px);
void HMC5883L_ST(signed short int * MAG_data);
void HMC5883L_SelfTest(void);
void HMC5883L_MAG_TASK(float * px);
void HMC5883L_SetMode(void);

#endif







