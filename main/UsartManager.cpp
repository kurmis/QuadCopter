#include "UsartManager.h"
//#include "main.h"
#include "stm32f30x.h"

#include "Mems.h"
#include "utils.h"
#include <stdio.h>

const int USART_SPEED= 115200;
bool RecievedData(int RX_Count) { return (RX_Count>0); };

//// command list  ///////
#define CMD_SIZE 5
int CMD_STR_LEN[CMD_SIZE];

char* CMD_STR[CMD_SIZE] = {"null","setSpeed","getData", "setAuto", "initMotors"};

void UsartManager::Init_Commands(void)
{
	int i = 0; 
	for(;i<CMD_SIZE; i++)
	{
		CMD_STR_LEN[i] = strlen(CMD_STR[i]);
	}
}



//////////////////////////




char command[127];
char args[128];

GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

UsartManager::UsartManager(int inten, Motors* motors) {
	m_motors = motors;
	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	/* Enable USART clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 
	/* Connect PXx to USARTx_Tx */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_7); 	
	/* Connect PXx to USARTx_Rx */ 
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_7);
	 /* Configure USART Tx as alternate function push-pull */

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function push-pull */ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	/* USART configuration */
	
	USART_InitStructure.USART_BaudRate = USART_SPEED; 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure); 

	if(inten == 1) 
	{
		/* Enable the USART2 Receive interrupt: this interrupt is generated when the
								 USART2 receive data register is not empty */
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
		/* Enable the USART2 Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

	}
	USART_Cmd(USART2, ENABLE);
	// enable RX interupt
	USART_ITConfig(USART2, USART_IT_TC, ENABLE);
	Init_Commands();
}

int UsartManager::GetCommand(volatile char* buffer, int size, Sensors* sensors)
{
	int motor = 0;
	int mspeed = 0;
	int cmd = 0; // command to execute
	int start = 0; //index of start
	int end = 0; // index of end
	while(buffer[start] != '!')
	{
		if(start >= size)
		{
			break;
		}
		start++;
	}	
	if(buffer[start] == '!')
	{
		//printf("found command start\n\r");
		end = start+1;
		while(buffer[end] != ';')
		{
			if(end == start)
			{
				break;
			}
			end++;
			if(end >= size)
			{
				end = 0;
			}
		}
		if(buffer[end] == ';')
		{
			//printf("found command end\n\ridentifying command\n\r");
			//printf("start at %d, end at %d\n\r", start, end);
			//identify
			cmd = Identify(buffer, size, start, end);
			if(cmd)
			{
				if(cmd == 1)
				{
					sscanf(args, " %d %d", &motor, &mspeed);
					if(0 == motor)
					{
						printf("setting all motors speed=%d\n\r", mspeed);
						for(motor = 1; motor < 5; motor++)
						{
							m_motors->SetSpeed(mspeed);
						}
					}
					else
					{
						printf("setting motor=%d speed=%d\n\r", motor, mspeed);
						if(motor == 1)
						{
							m_motors->motor1->SetSpeed(mspeed);
						}
					}
				}
				else if(cmd == 2)
				{
					float angles[3] = {0};
					sensors->GetAngles(angles);
					printf("angles: %g %g %g\n\r",angles[0],angles[1],angles[2]); 
					
				}
				/*else if (cmd == 3)
				{
					float mspeed = 200.0f;
					float gyroData[3]={0.0f};
					float delta = 0.0f;
					while(1)
					{
						Demo_GyroReadAngRate(gyroData);
						//delta = (200-accData[0]) / 5;
						delta = gyroData[0]*10;
						m_motors->motor1->SetSpeed(mspeed+delta);
						Delay(300);
						printf("delta %g, speed %g\n\r",delta,delta+mspeed);
					}
					
				}*/
				else if(cmd == 4)
				{
					printf("Restarting motors");
					m_motors->Reinit();
				}
				else
				{
					printf("command not implemented\n\r");
				}
			}
			else
			{
				printf("unidentified command\n\r");
			}
			Clear_Buffer(buffer, size);
		}
	}
	return cmd;
}

void UsartManager::Clear_Buffer(volatile char* buffer, int size)
{
	int i = 0;
	for(i = 0; i < size; i++)
	{
		buffer[i] = 0;
	}
}

int UsartManager::Identify(volatile char* buffer, int b_size, int start, int stop)
{
	int compareValue = 0;
	int cmd = 0;
	int i = start+1;
	int size = 0;
	while(i != stop)
	{
		command[size++] = buffer[i++];
		if(i > b_size)
		{
			i = 0;
		}
	}
	command[size++] = '\0';
	//printf("Command extracted '%s', size %d \n\r", command, size);
	for(;cmd < CMD_SIZE; cmd++)
	{
		compareValue = compStr(command, CMD_STR[cmd], CMD_STR_LEN[cmd]); 
		//printf("Comparing to command '%s', compare value %d\n\r", CMD_STR[cmd], compareValue);
		if(0 != compareValue)
		{
			int index = 0;
			int pointer = CMD_STR_LEN[cmd];
			while(pointer != size)
			{
				args[index++] = command[pointer++];
			}				
			//printf("extracted args string '%s'\n\r", args);
			return cmd;
		}
	}
	return 0;
}

int UsartManager::compStr (char *s1, char *s2, int sz) {
    while (sz != 0) {
        // At end of both strings, equal.
        if ((*s1 == '\0') && (*s2 == '\0')) break;

        // Treat spaces at end and end-string the same.
        if ((*s1 == '\0') && (*s2 == ' ')) { s2++; sz--; continue; }
        if ((*s1 == ' ') && (*s2 == '\0')) { s1++; sz--; continue; }

        // Detect difference otherwise.
        if (*s1 != *s2) return 0;

        s1++; s2++; sz--;
    }
    return 1;
}
