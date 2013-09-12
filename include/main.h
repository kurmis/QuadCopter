
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/



static void TIM_Config(void);


//extern __IO uint8_t RX_Buffer[128];
//extern __IO uint8_t TX_Buffer[128];


void SysTick_Handler (void);
void doCalc(float dt);

#endif /* __MAIN_H */
