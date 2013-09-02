/* Includes ------------------------------------------------------------------*/

#include "utils.h"
#include "main.h"
#include "MotorControl.h"

uint32_t volatile msTicks;
__IO uint32_t UserButtonPressed = 0;
uint32_t LEDS[LED_NUM] = {LED3,LED4,LED6,LED8,LED10,LED9,LED7,LED5};


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
	STM_EVAL_LEDOn(LEDS[LED_NUM-1]);
	UserButtonPressed = 0x00;
  while (1)
  {
		if(UserButtonPressed == 0x00)
		{
			STM_EVAL_LEDOn(LEDS[0x0]);
			setMotorSpeed(1, 0.0f);
			while(UserButtonPressed == 0x00)
			{
				;
			}
			STM_EVAL_LEDOff(LEDS[0x0]);
		}
		if (UserButtonPressed == 0x01)
		{
			float level = 0.0f;
			STM_EVAL_LEDOn(LEDS[0x1]);
			setMotorSpeed(1, 300);
			while(UserButtonPressed == 0x01)
			{
				level++;
				//setMotorSpeed(1, level);
				if(level > 1000.0f)
				{
					level = 0.0f;
				}
				Delay(700);
			}
			STM_EVAL_LEDOff(LEDS[0x1]);
		}
		if(UserButtonPressed == 0x02)
		{
			STM_EVAL_LEDOn(LEDS[0x2]);
			setMotorSpeed(1, 400.0f);
		  while(UserButtonPressed == 0x02)
			{
				;
			}
			STM_EVAL_LEDOff(LEDS[0x2]);
		}
		if(UserButtonPressed == 0x03)
		{
			STM_EVAL_LEDOn(LEDS[0x3]);
			setMotorSpeed(1, 500.0f);
		  while(UserButtonPressed == 0x03)
			{
				;
			}
			STM_EVAL_LEDOff(LEDS[0x3]);
		}
		if(UserButtonPressed == 0x04)
		{
			STM_EVAL_LEDOn(LEDS[0x4]);
			setMotorSpeed(1, 800.0f);
		  while(UserButtonPressed == 0x03)
			{
				;
			}
			STM_EVAL_LEDOff(LEDS[0x4]);
		}
		
  }//while(1)
}



