/* Includes ------------------------------------------------------------------*/
#include "main.h"

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#define UART_FLAG_TimeOut  0x50000   //��ʱ����
	
volatile unsigned char ModbusIntervalTime = Default_MODBUS_INTERVAL_TIME;//ָ��ͼ��ʱ��,��ʱ��Ϊһ��ָ���ѽ������
	
volatile unsigned char USART1_INTERVAL_TIME = 0;//ָ��ͼ��ʱ��,��ʱ��Ϊһ��ָ���ѽ������	
volatile unsigned char USART1_RX_Len=0;     //���ڽ���ָ���
volatile unsigned char USART2_INTERVAL_TIME = 0;//ָ��ͼ��ʱ��,��ʱ��Ϊһ��ָ���ѽ������
volatile unsigned char USART2_RX_Len=0;     //���ڽ���ָ���
volatile unsigned char USART3_INTERVAL_TIME = 0;//ָ��ͼ��ʱ��,��ʱ��Ϊһ��ָ���ѽ������
volatile unsigned char USART3_RX_Len=0;     //���ڽ���ָ���

unsigned char USART1_RX[256];
unsigned char USART2_RX[256];
unsigned char USART3_RX[256];	

unsigned char Uart1Flag;
UART1_RBUF_ST	uart1_rbuf	=	{ 0, 0, };

unsigned char Uart2Flag;
UART2_RBUF_ST	uart2_rbuf	=	{ 0, 0, };

unsigned char Uart3Flag;
UART3_RBUF_ST	uart3_rbuf	=	{ 0, 0, };

/**********************************************************************************
* Function Name  : UART1_Configuration
* ����1��ʼ��
* ��ڲ���
* baudrate:������
**********************************************************************************/
void UART1_Configuration(unsigned int baudrate)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable the USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//��GPIO��USART1��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ�ܴ���1ʱ��
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_USART1);//�˿���ӳ��
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_USART1);
	
	/* ��USART1_Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;     //GPIO_Pin_6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/*  ��USART1_Rx��GPIO����Ϊ��������ģʽ
  	����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
  	���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò��� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;		//GPIO_Pin_7
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = baudrate;//���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//��������λΪ8
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//����ֹͣλΪ1
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������λ
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//���ͺͽ���
	USART_Init(USART1, &USART_InitStructure);
	
	//�ж�ʹ��
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* �巢�ͱ�־��Transmission Complete flag */
	
	//ʹ��
	USART_Cmd(USART1, ENABLE);
	
}

/**********************************************************************************
* Function Name  : UART2_Configuration
* ����2��ʼ��
* ��ڲ���
* baudrate:������
**********************************************************************************/
void UART2_Configuration(unsigned int baudrate)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable the USART2 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//��GPIO��USART2��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	
	/* ��USART2_Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;     //GPIO_Pin_2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	/*  ��USART2_Rx��GPIO����Ϊ��������ģʽ
  	����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
  	���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò��� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;		//GPIO_Pin_3
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = baudrate;//���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//��������λΪ8
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//����ֹͣλΪ1
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������λ
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//���ͺͽ���
	USART_Init(USART2, &USART_InitStructure);
	
	
	//�ж�ʹ��
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ClearFlag(USART2, USART_FLAG_TC);     /* �巢�ͱ�־��Transmission Complete flag */
	
	//ʹ��
	USART_Cmd(USART2, ENABLE);
	
}

/**********************************************************************************
* Function Name  : UART3_Configuration
* ����3��ʼ��
* ��ڲ���
* baudrate:������
**********************************************************************************/
void UART3_Configuration(unsigned int baudrate)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable the USART3 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//��GPIO��USART3��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
	
	/* ��USART3_Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;     //GPIO_Pin_10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/*  ��USART3_Rx��GPIO����Ϊ��������ģʽ
  	����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
  	���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò��� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		//GPIO_Pin_3
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = baudrate;//���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//��������λΪ8
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//����ֹͣλΪ1
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������λ
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//���ͺͽ���
	USART_Init(USART3, &USART_InitStructure);
	
	
	//�ж�ʹ��
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
	USART_ClearFlag(USART3, USART_FLAG_TC);     /* �巢�ͱ�־��Transmission Complete flag */
	
	//ʹ��
	USART_Cmd(USART3, ENABLE);
	
}


//void UART1TX_DMA_Configuration( void )
//{
//	DMA_InitTypeDef DMA_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//  uint32_t temp;
//	/* DMA1 channel4 configuration ---------------------------------------------*/
//  /* Enable DMA1 clock --------------------------------------------------------*/
//	/*DMA1 channel4,USART1_TX*/
//  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
//  DMA_DeInit(DMA1_Channel4);//����DMA1�ĵ���ͨ������ӦUSART1_TX
//	
//  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);//DMA��Ӧ�������ַ
//  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&temp;//�ڴ滺���ַ
//  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;//DMA��ת��ģʽΪDSTģʽ�����ڴ���Ƶ�����
//  DMA_InitStructure.DMA_BufferSize = 1;//DMA�����С��������N���ֽڵ����ݺ��Զ������ж�
//  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//����һ�����ݺ��豸��ַ��ֹ����
//  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�ڴ��ַ����
//  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//DMA�������ݳߴ磬Byte����Ϊ8λ
//  DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
//  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//����ģʽ
//  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
//  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//�ڴ浽�ڴ�ʧ��
//  DMA_Init(DMA1_Channel4, &DMA_InitStructure);

//	/* Enable DMA1 channel4 IRQ Channel */
//  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
//	/* Enable USART1_TX request */
//	
//  /* Enable DMA1 Channel4 Transfer Complete interrupt */
//  DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
//	//DMA_ITConfig(DMA1_Channel4, DMA_IT_TE, ENABLE);
//	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);  //���ջ�����DMAʹ��
// 	//DMA_Cmd(DMA1_Channel4,  DISABLE);
//}


//void UART1_DMA_SendData(unsigned char *BufAddress,unsigned int Length)
//{
//  DMA_InitTypeDef DMA_InitStructure;
//  
//  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DR);//�����ַ
//  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)BufAddress;//�����ַ
//  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;//�ڴ浽����
//  DMA_InitStructure.DMA_BufferSize = Length;//����buf��С
//  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//�����ַ������
//  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�ڴ��ַ����
//  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//8bit
//  DMA_InitStructure.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
//  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//����ģʽ
//  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
//  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//�ڴ浽�ڴ�ʧ��
//  DMA_Init(DMA1_Channel4, &DMA_InitStructure);
//  
//  DMA_Cmd(DMA1_Channel4,  ENABLE);
//  //USART_Cmd(USART1, ENABLE);
//}



/**********************************************************************************
* Function Name  : USART_PutChar
* ���ڷ���һ���ַ�
* ��ڲ���
* USARTx:���ں�
* ch:����
**********************************************************************************/
void USART_PutChar(USART_TypeDef* USARTx,unsigned char ch)
{
  unsigned int timeout = 0;
	USART_GetFlagStatus(USARTx, USART_FLAG_TC);//USARTx->SR  ����豸�������ϵ磬���ڵ�һ�η����������ֽڶ�ʧ����
																						 //��ȡһ��SR�Ĵ��������TC��־λ��
  USART_SendData(USARTx, (uint8_t) ch);
  while((USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET) && (timeout++ < UART_FLAG_TimeOut));
}
/**********************************************************************************
* Function Name  : USART_PutData
* ���ڷ������ɸ��ַ�
* ��ڲ���
* USARTx:���ں�
* ch:����
* len:���ݳ���
**********************************************************************************/
void USART_PutData(USART_TypeDef* USARTx,unsigned char *dat,unsigned  int len)
{
	unsigned  int i;
	for(i = 0;i < len;i++)USART_PutChar(USARTx,(uint8_t)* (dat++));
}
/**********************************************************************************
* Function Name  : USART_PutS
* ���ڷ����ַ���
* ��ڲ���
* USARTx:���ں�
* *s:����ָ��
**********************************************************************************/
void USART_PutS(USART_TypeDef* USARTx,unsigned char *s)
{
	while(*s != '\0')USART_PutChar(USARTx,*(s++));
}

/**********************************************************************************
* ����1�����ַ�����������ģʽ�����ջ���������ȡ��
**********************************************************************************/
uint16_t USART1_GetCharBlock(uint16_t timeout)
{
	UART1_RBUF_ST *p = &uart1_rbuf;
	uint16_t to = timeout;	
	while(p->out == p->in)if(!(--to))return TIMEOUT;
	return (p->buf [(p->out++) & (UART1_RBUF_SIZE - 1)]);
}

/**********************************************************************************
* ����1�����ַ�������������ģʽ�����ջ���������ȡ��
**********************************************************************************/
uint16_t USART1_GetChar(void)
{
	UART1_RBUF_ST *p = &uart1_rbuf;			
	if(p->out == p->in) //������������
		return EMPTY;
	return USART1_GetCharBlock(1000);
}

/**********************************************************************************
* ����2�����ַ�����������ģʽ�����ջ���������ȡ��
**********************************************************************************/
uint16_t USART2_GetCharBlock(uint16_t timeout)
{
	UART2_RBUF_ST *p = &uart2_rbuf;
	uint16_t to = timeout;	
	while(p->out == p->in)if(!(--to))return TIMEOUT;
	return (p->buf [(p->out++) & (UART2_RBUF_SIZE - 1)]);
}

/**********************************************************************************
* ����2�����ַ�������������ģʽ�����ջ���������ȡ��
**********************************************************************************/
uint16_t USART2_GetChar(void)
{
	UART2_RBUF_ST *p = &uart2_rbuf;			
	if(p->out == p->in) //������������
		return EMPTY;
	return USART2_GetCharBlock(1000);
}

/**********************************************************************************
* ����3�����ַ�����������ģʽ�����ջ���������ȡ��
**********************************************************************************/
uint16_t USART3_GetCharBlock(uint16_t timeout)
{
	UART3_RBUF_ST *p = &uart3_rbuf;
	uint16_t to = timeout;
	while(((p->out - p->in)& (UART3_RBUF_SIZE - 1)) == 0)if(!(--to))return TIMEOUT;
	return (p->buf [(p->out++) & (UART3_RBUF_SIZE - 1)]);
}

/**********************************************************************************
* ����3�����ַ�������������ģʽ�����ջ���������ȡ��
**********************************************************************************/
uint16_t USART3_GetChar(void)
{
	UART3_RBUF_ST *p = &uart3_rbuf;		
	if(((p->out - p->in) & (UART3_RBUF_SIZE - 1)) == 0) //������������
		return EMPTY;
	return USART3_GetCharBlock(1000);
}

/**********************************************************************************
printf���ܶ��壬(�����б�ʾ���ĸ�����������ݼ���������)
**********************************************************************************/
PUTCHAR_PROTOTYPE
{
	USART_PutChar(USART1,(uint8_t) ch);
	return ch;
}

/**********************************************************************************
�������1���ձ�־λ����֡����
**********************************************************************************/
void USART1_ClearBuf_Flag(void)
{
	UART1_RBUF_ST *p = &uart1_rbuf;
	p->out = 0;
	p->in = 0;
	
	Uart1Flag = 0;
	USART1_RX_Len = 0;
}

/**********************************************************************************
�������2���ձ�־λ����֡����
**********************************************************************************/
void USART2_ClearBuf_Flag(void)
{
	UART2_RBUF_ST *p = &uart2_rbuf;
	p->out = 0;
	p->in = 0;
	
	Uart2Flag = 0;
	USART2_RX_Len = 0;
	//ModbusDataPackage.DataFlag=0;
	//ModbusDataPackage.DataLen=0;
}

/**********************************************************************************
�������3���ձ�־λ����֡����
**********************************************************************************/
void USART3_ClearBuf_Flag(void)
{
	UART3_RBUF_ST *p = &uart3_rbuf;
	p->out = 0;
	p->in = 0;
	
	Uart3Flag = 0;
	USART3_RX_Len = 0;
}


void U485SendData(unsigned char *dat,unsigned short len)
{
	RS485_TX();
	__nop();
	USART_PutData(USART1,dat,len);
	__nop();
	RS485_RX();
}

//void U485_2SendData(unsigned char *dat,unsigned short len)
//{
//	//RS485_TX();
//	//__nop();
//	USART_PutData(USART2,dat,len);
//	//__nop();
//	//RS485_RX();
//}



















