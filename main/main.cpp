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
#include "Clock.h"
#include "QuadControl.h"

uint32_t LEDS[LED_NUM] = {LED3,LED4,LED6,LED8,LED10,LED9,LED7,LED5};
__IO char RX_Buffer[128];
__IO char TX_Buffer[128];
extern volatile uint32_t UserButtonPressed;
volatile uint32_t msTicks = 0;
Sensors* sensors;
Motors* motors;
UsartManager* usart;
bool balance = false;


int main(void)
{	
	int i = 0;
 
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_EXTI);
	for(i = 0; i < LED_NUM; i++)
	{
		STM_EVAL_LEDInit((Led_TypeDef)LEDS[i]);
	}
	if (SysTick_Config(SystemCoreClock / 1000))
  { 
    /* Capture error */ 
    while (1);
  }
	Clock clock;
	
	for(i = 0; i < LED_NUM; i++)
	{
		STM_EVAL_LEDOff((Led_TypeDef)LEDS[i]);
	}
	
	Motors motors_(false);
	motors = &motors_;
	Gyro gyro(1);
	Compass compass(1);
	Sensors sensors_(&gyro, &compass);
	sensors = &sensors_;
	
	int period = motors->GetPwmPeriod();
	//pd13 
	Motor motor1(1U, period);
	Motor motor2(2U, period);
	Motor motor3(3U, period);
	Motor motor4(4U, period);
	motors->motor1 = &motor1;
	motors->motor2 = &motor2;
	motors->motor3 = &motor3;
	motors->motor4 = &motor4;
	QuadControl control(sensors, motors);
	UsartManager usart_(1, motors, sensors, &control);
	usart = &usart_;
	STM_EVAL_LEDOn(LED10);
	UserButtonPressed = 0x00;
	printf("Started\n\rType !help; for available commands\n\r");
	volatile unsigned int oldTicks = msTicks;
	float balanceZero[3] = {0};
	//sensors->Calibrate(0, 20);
	Sleeper sleep;
  while (1)
  {
		//printf("Time %d, delta %d", clock.GetTime(), clock.GetTime() - oldTime);
		//printf("msTicks delta %d", msTicks - oldTicks);
		for(i = 0; i < 13; i++)
		{
			sleep.FromNow();
			doCalc(oldTicks);
			oldTicks = msTicks;
			usart->GetCommand(RX_Buffer, 128);	
			sleep.For(15);
			//Delay(20);
		}
		if(balance)
		{
			control.Balance(balanceZero);
		}
  }//while(1)
}

void doCalc(float dt)
{
	sensors->CalcAngles(dt);
}

void setAuto()
{
	balance = !balance;
}
int getAuto()
{
	return balance;
}

