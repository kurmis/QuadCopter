
#include "utils.h"


void Delay(__IO uint32_t nTime)
{
	uint32_t delayedTime = msTicks + nTime;
	while(delayedTime > msTicks);
}

uint8_t strlen(char* str)
{
	uint8_t i;
	for(i = 0; str[i]; i++)
		;
	return i;
}


