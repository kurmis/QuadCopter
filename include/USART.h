#ifndef __USART_H
#define __USART_H

#include <stdio.h>
//#include "main.h"

#define speed 115200
#define RecievedData (RX_Count>0)




void Init_USART(int inten);
void Analyze_Usart(volatile char * buffer, int size);
void Clear_Buffer(volatile char* buffer, int size);
int Identify(volatile char* buffer, int size, int start, int stop);
int compStr (char *s1, char *s2, int sz);
void Init_Commands(void);

#endif
