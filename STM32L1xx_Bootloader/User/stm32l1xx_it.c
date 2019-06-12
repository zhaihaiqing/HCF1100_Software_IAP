/**
  ******************************************************************************
  * @file    Project/STM32L1xx_StdPeriph_Templates/stm32l1xx_it.c 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    16-May-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_it.h"
#include "main.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	//SysTick_Count++;
	if(TimingDelay)TimingDelay--;//延时函数
	
	if(ModbusIntervalTime) ModbusIntervalTime--;
	else
	{
		if(ModbusDataPackage.DataLen && !ModbusDataPackage.DataFlag)
		ModbusDataPackage.DataFlag = 1;
		Modbus_FreeFlag=0;
	}
}


void USART1_IRQHandler(void)
{
	unsigned char 	Uart_Get_Data;	//串口1接收的数据
	UART1_RBUF_ST *p = &uart1_rbuf;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		Modbus_BusyFlag=1;
		Modbus_FreeFlag=1;
		Uart_Get_Data = USART_ReceiveData(USART1);
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		if(!ModbusDataPackage.DataFlag)
		{
			//如果缓存为空,表示第一个数据,或者未达到接收时间间隔，判定为继续接收
			if(((p->out - p->in) == 0) || ModbusIntervalTime)
			{
				ModbusIntervalTime=Default_USART1_INTERVAL_TIME;
				if((p->in - p->out)<UART1_RBUF_SIZE)
				{
					p->buf [p->in & (UART1_RBUF_SIZE-1)] = Uart_Get_Data;	
					p->in++;
				}
				//USART1_RX_Len  = (p->in - p->out) & (UART1_RBUF_SIZE - 1);//获取数据长度
				ModbusDataPackage.DataLen  = (p->in - p->out);//获取数据长度
			}
			
		}
	}
}




void USART2_IRQHandler(void)
{
	unsigned char 	Uart_Get_Data;	//串口2接收的数据
	UART2_RBUF_ST *p = &uart2_rbuf;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		Uart_Get_Data = USART_ReceiveData(USART2);
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		if(!Uart1Flag)
		{
			if((((p->out - p->in) & (UART2_RBUF_SIZE - 1)) == 0) || USART2_INTERVAL_TIME)
			{
				USART2_INTERVAL_TIME=Default_USART2_INTERVAL_TIME;
				if((p->in - p->out)<UART2_RBUF_SIZE)
				{
					p->buf [p->in & (UART2_RBUF_SIZE-1)] = Uart_Get_Data;	
					p->in++;
				}
				USART2_RX_Len  = (p->in - p->out) & (UART2_RBUF_SIZE - 1);//获取数据长度
			}
			
		}
	}
}

void USART3_IRQHandler(void)
{
	unsigned char 	Uart_Get_Data;	//串口2接收的数据
	UART3_RBUF_ST *p = &uart3_rbuf;
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		Uart_Get_Data = USART_ReceiveData(USART3);
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		if(!Uart3Flag)
		{
			if((((p->out - p->in) & (UART3_RBUF_SIZE - 1)) == 0) || USART3_INTERVAL_TIME)
			{
				USART3_INTERVAL_TIME=Default_USART3_INTERVAL_TIME;
				if((p->in - p->out)<UART3_RBUF_SIZE)
				{
					p->buf [p->in & (UART3_RBUF_SIZE-1)] = Uart_Get_Data;	
					p->in++;
				}
				USART3_RX_Len  = (p->in - p->out) & (UART3_RBUF_SIZE - 1);//获取数据长度
			}
			
		}
	}
}

/******************************************************************************/
/*                 STM32L1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l1xx_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
