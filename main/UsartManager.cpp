#include "UsartManager.h"
//#include "main.h"
#include "stm32f30x.h"

#include "Mems.h"
#include "utils.h"
#include <stdio.h>

const int USART_SPEED= 115200;
bool RecievedData(int RX_Count) { return (RX_Count>0); };
extern float MIN_SPEED;

//// command list  ///////
const int CMD_SIZE = 11;
int CMD_STR_LEN[CMD_SIZE];

char* CMD_STR[CMD_SIZE] = {"null","setSpeed","getData", "setAuto", "initMotors", "setPID", "help", "setPrint", "setMinSpeed", "getPID", "getSpeed"};

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

UsartManager::UsartManager(int inten, Motors* motors, Sensors* sensors, QuadControl* control) {
	m_motors = motors;
	m_sensors = sensors;
	m_control = control;
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

int UsartManager::GetCommand(volatile char* buffer, int size)
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
							m_control->SetThrottle(mspeed);
						}
					}
					else
					{
						printf("setting motor=%d speed=%d\n\r", motor, mspeed);
						switch(motor) {
							case 1: m_motors->motor1->SetSpeed(mspeed); break;
							case 2: m_motors->motor2->SetSpeed(mspeed); break;
							case 3: m_motors->motor3->SetSpeed(mspeed); break;
							case 4: m_motors->motor4->SetSpeed(mspeed); break;
							default: printf("no motor %d\n\r", motor); break;
						}
							
					}
				}
				else if(cmd == 2)
				{
					float angles[3] = {0};
					m_sensors->GetAngles(angles);
					
					printf("angles: %g %g %g\n\r",angles[0],angles[1],angles[2]); 
					//TransmitAngles(angles);
				}
				else if (cmd == 3)
				{
					setAuto();
					printf("Auto balance is %d\n\r", getAuto());
					if(!getAuto())
					{
						m_motors->SetSpeed(0);
					}
				}
				else if(cmd == 4)
				{
					printf("Restarting motors\n\r");
					m_motors->Reinit();
				}
				else if(cmd == 5)
				{
					char pid[255] = {0};
					float p;
					float i;
					float d;
					sscanf(args, "%s %g %g %g", pid, &p, &i, &d);
					printf("Setting pid: %s Kp = %g Ki = %g Kd = %g\n\r", pid, p, i , d);
					if(pid[0] == 'x')
					{
						m_control->SetPIDX(p, i, d);
						printf("Success\n\r");
					}
					else if(pid[0] == 'y')
					{
						m_control->SetPIDY(p, i, d);
						printf("Success\n\r");
					}
					else
					{
						printf("PID '%s' not found\n\r", pid);
					}
					
				}
				else if(cmd == 6)
				{
					printf("Available commands: \n\r");
					for(int index = 0; index < CMD_SIZE; index++)
					{
						printf("!%s;\n\r", CMD_STR[index]);
					}
					printf("\n\r");
				}
				else if(cmd == 7)
				{
					m_control->TogglePrint();
					printf("PID print %d\n\r", m_control->GetPrint()); 
					printf("Mems print %d\n\r", m_sensors->GetPrint());
				}
				else if (cmd == 8)
				{
					float minSpeed = MIN_SPEED;
					sscanf(args, "%g", &minSpeed);
					MIN_SPEED = minSpeed;
					printf("Min motor speed = %g\n\r", MIN_SPEED);
				}
				else if(cmd == 9)
				{
					m_control->PrintPIDVals();
				}
				else if(cmd == 10)
				{
					printf("Auto throttle is at %g\r\n", m_control->GetThrottle());
				}
				else
				{
					printf("command not implemented (cmd = %d)\n\r", cmd);
				}
			}
			else
			{
				printf("unidentified command, type !help; to see comand list\n\r");
			}
			Clear_Buffer(buffer, size);
		}
	}
	return cmd;
}

void UsartManager::TransmitAngles(float* angles)
{
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			uint8_t const * byte = reinterpret_cast<uint8_t const * >(&angles[i]);
			USART_SendData(USART2, byte[j]);								
			 // Wait until transmit finishes 
			while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		}
	}
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
		if(CMD_STR_LEN[cmd] > 20)
		{
			printf("error in cmd %d len: %d commnad string %s.. reiniting\r",cmd, CMD_STR_LEN[cmd], CMD_STR[cmd]);
      Init_Commands();			
		}
		compareValue = compStr(command, CMD_STR[cmd], CMD_STR_LEN[cmd]); 
		//printf("Comparing to command '%s', compare value %d, size = %d\n\r", CMD_STR[cmd], compareValue, CMD_STR_LEN[cmd]);
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
	  //printf("Comparing %s to %s\n\r", s1, s2);
    while (sz != 0) {
        // At end of both strings, equal.
        if ((*s1 == '\0') && (*s2 == '\0')) break;

        // Treat spaces at end and end-string the same.
        if ((*s1 == '\0') && (*s2 == ' ')) { s2++; sz--; continue; }
        if ((*s1 == ' ') && (*s2 == '\0')) { s1++; sz--; continue; }

        // Detect difference otherwise.
				//printf("size = %d, %s %s\r", sz, s1, s2);
        if (*s1 != *s2) return 0;

        s1++; s2++; sz--;
    }
    return 1;
}

