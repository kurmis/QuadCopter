/**
  ******************************************************************************
  * @file    SysTick_Example/stm32f30x_it.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    20-September-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
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
#include "stm32f30x_it.h"
#include "main.h"

/** @addtogroup STM32F3_Discovery_Peripheral_Examples
  * @{
  */

/** @addtogroup SysTick_Example
  * @{
  */
extern __IO uint8_t DataReady;
extern __IO uint32_t USBConnectTimeOut;
extern uint8_t Data = 0;
extern char TX_Buffer[128];
extern char RX_Buffer[128];

uint8_t TX_OUT_Index 	= 0;
uint8_t TX_IN_Index 	= 0;
uint8_t TX_Count 	= 0;
 
uint8_t RX_OUT_Index 	= 0;
uint8_t RX_IN_Index 	= 0;
uint8_t RX_Count 	= 0;
__IO uint32_t i = 0;
__IO uint32_t m = 0;
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
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
	msTicks++;
}

/******************************************************************************/
/*                 STM32F30x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f30x.s).                                            */
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

/**
  * @}
  */ 
void EXTI0_IRQHandler(void)
{ 
	int i;
  if ((EXTI_GetITStatus(USER_BUTTON_EXTI_LINE) == SET)&&(STM_EVAL_PBGetState(BUTTON_USER) != RESET))
  {
    /* Delay */
    for(i=0; i<0x7FFFF; i++);
    
    /* Wait for SEL button to be pressed  */
    while(STM_EVAL_PBGetState(BUTTON_USER) != RESET); 
    /* Delay */
    for(i=0; i<0x7FFFF; i++);
    UserButtonPressed++;
    
    if (UserButtonPressed > 0x4	)
    {
      UserButtonPressed = 0x0;
    }
    
    /* Clear the EXTI line pending bit */
    EXTI_ClearITPendingBit(USER_BUTTON_EXTI_LINE);
  }
}
void USART2_IRQHandler()
{
	//RX Interrupt Mode
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{		
		if(USART_GetFlagStatus(USART2,USART_FLAG_PE|USART_FLAG_FE|USART_FLAG_ORE)==0)
		{
			RX_Buffer[RX_IN_Index]=USART_ReceiveData(USART2);
			if (++RX_IN_Index == sizeof(RX_Buffer)) RX_IN_Index = 0;
			if (++RX_Count == sizeof(RX_Buffer))		RX_Count = 0;
		}
		USART_ClearITPendingBit(USART2,USART_IT_RXNE); 
	}	
	
	//TX_Buffer Interrupt Mode
	if(USART_GetITStatus(USART2, USART_IT_TC) != RESET)
	{
		RX_IN_Index = 0;
		if(TX_Count)
		{	
			--TX_Count;
			USART_SendData(USART2, TX_Buffer[TX_OUT_Index]); 
			if(++TX_OUT_Index == sizeof(TX_Buffer)) 
				TX_OUT_Index=0;						 
		}
		USART_ClearITPendingBit(USART2,USART_IT_TC);
	}		
	
} 

int fgetc(FILE *f)
{
	char data;
	while (RX_Count==0);
	data=RX_Buffer[RX_OUT_Index];
	if (++RX_OUT_Index == sizeof(RX_Buffer)) 
		RX_OUT_Index=0;
	--RX_Count;
	return data;
}
int fputc(int ch, FILE * f)
{
  // Transmit the character using USART1 
  USART_SendData(USART2, (u8) ch);

   // Wait until transmit finishes 
  while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);

   return ch;
} 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
