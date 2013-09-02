
#include "utils.h"


void Delay(__IO uint32_t nTime)
{
	uint32_t delayedTime = msTicks + nTime;
	while(delayedTime > msTicks);
}


