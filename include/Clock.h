#ifndef __CLOCK_H
#define __CLOCK_H
#include "stm32f30x.h"
#include "stm32f3_discovery.h"
#include "stm32f30x_it.h"


class Clock {
private:
	RTC_DateTypeDef m_RTCDateStructure;
  RTC_TimeTypeDef m_RTCTimeStructure;
  RTC_InitTypeDef m_RTCInitStructure;
public:
	Clock();
  int GetTime();
	
};






#endif
