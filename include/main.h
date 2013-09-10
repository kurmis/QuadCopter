
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f30x.h"
#include "stm32f30x_rcc.h"
#include "stm32f3_discovery.h"
#include "stm32f3_discovery_lsm303dlhc.h"
#include "stm32f3_discovery_l3gd20.h"
#include "math.h"
#include "UsartCtrl.h"
#include <stdio.h>

static void TIM_Config(void);


//extern __IO uint8_t RX_Buffer[128];
//extern __IO uint8_t TX_Buffer[128];

extern volatile uint32_t UserButtonPressed;
extern uint32_t volatile msTicks;
void SysTick_Handler (void);
void Demo_CompassConfig(void);
void Demo_CompassReadMag (float* pfData);
void Demo_CompassReadAcc(float* pfData);
void Demo_GyroConfig(void);
void Demo_GyroReadAngRate (float* pfData);

#endif /* __MAIN_H */
