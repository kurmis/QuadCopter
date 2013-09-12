#include <stm32f30x.h>
/* Includes ------------------------------------------------------------------*/

#include "utils.h"
#include "main.h"
#include "MotorControl.h"
#include "Mems.h"
#include "UsartManager.h"
#include <stdio.h>
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f3_discovery.h"
#include "stm32f3_discovery_lsm303dlhc.h"
#include "stm32f3_discovery_l3gd20.h"
#include <math.h>

uint32_t volatile msTicks;
uint32_t LEDS[LED_NUM] = {LED3,LED4,LED6,LED8,LED10,LED9,LED7,LED5};
__IO char RX_Buffer[128];
__IO char TX_Buffer[128];
extern volatile uint32_t UserButtonPressed;
extern uint32_t volatile msTicks;
Sensors* sensors;

int main(void)
{	
	int i = 0;
	//uint32_t led_num = 0;  
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
	for(i = 0; i < LED_NUM; i++)
	{
		STM_EVAL_LEDInit((Led_TypeDef)LEDS[i]);
	}
	
	msTicks = 0;	
	//__enable_irq ();
  
	// Config the PWM freq to 200Hz
		
	
	if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
    while (1);
  }
	
	for(i = 0; i < LED_NUM; i++)
	{
		STM_EVAL_LEDOff((Led_TypeDef)LEDS[i]);
	}
	Sensors sensors_;
	sensors = &sensors_;
	Motors motors(false);
	UsartManager usart(1, &motors);
	STM_EVAL_LEDOn((Led_TypeDef)LEDS[LED_NUM-1]);
	UserButtonPressed = 0x00;
	printf("Init complete\n\r");
  while (1)
  {
		usart.GetCommand(RX_Buffer, 128, sensors);		
  }//while(1)
}

