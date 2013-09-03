/* Includes ------------------------------------------------------------------*/

#include "utils.h"
#include "main.h"
#include "MotorControl.h"

uint32_t volatile msTicks;
__IO uint32_t UserButtonPressed = 0;
uint32_t LEDS[LED_NUM] = {LED3,LED4,LED6,LED8,LED10,LED9,LED7,LED5};
__IO char RX_Buffer[128];
__IO char TX_Buffer[128];

int main(void)
{	
	int i = 0;
	//uint32_t led_num = 0;  
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
	for(i = 0; i < LED_NUM; i++)
	{
		STM_EVAL_LEDInit(LEDS[i]);
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
		STM_EVAL_LEDOff(LEDS[i]);
	}
	init_motors(false);
	Init_USART(1);
	STM_EVAL_LEDOn(LEDS[LED_NUM-1]);
	UserButtonPressed = 0x00;
	printf("Init complete\n\r");
  while (1)
  {
		Analyze_Usart(RX_Buffer, 128);		
  }//while(1)
}

