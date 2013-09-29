#include "Clock.h"



Clock::Clock()
{
	/* Enable PWR APB1 Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  
  /* Allow access to Backup */
  PWR_BackupAccessCmd(ENABLE);
  
  /* Reset RTC Domain */
  RCC_BackupResetCmd(ENABLE);
  RCC_BackupResetCmd(DISABLE);
  
  /* Allow access to RTC */
  PWR_BackupAccessCmd(ENABLE);
  
  /* The RTC Clock may varies due to LSI frequency dispersion */   
  /* Enable the LSI OSC */ 
  RCC_LSICmd(ENABLE);
  
  /* Wait till LSI is ready */  
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
  {
  }
  
  /* Select the RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
  
  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);
  
  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();

  /* RTC prescaler configuration */
  m_RTCInitStructure.RTC_HourFormat = RTC_HourFormat_24;
  m_RTCInitStructure.RTC_AsynchPrediv = 88;
  m_RTCInitStructure.RTC_SynchPrediv = 470;
  RTC_Init(&m_RTCInitStructure);
	
	/* Set the date: Wednesday August 15th 2012 */
  m_RTCDateStructure.RTC_Year = 12;
  m_RTCDateStructure.RTC_Month = RTC_Month_September;
  m_RTCDateStructure.RTC_Date = 11;
  m_RTCDateStructure.RTC_WeekDay = RTC_Weekday_Tuesday;
  RTC_SetDate(RTC_Format_BCD, &m_RTCDateStructure);
  
  /* Set the time to 01h 00mn 00s AM */
  m_RTCTimeStructure.RTC_H12     = RTC_H12_AM;
  m_RTCTimeStructure.RTC_Hours   = 0x00;
  m_RTCTimeStructure.RTC_Minutes = 0x00;
  m_RTCTimeStructure.RTC_Seconds = 0x00; 
  RTC_SetTime(RTC_Format_BCD, &m_RTCTimeStructure);    
  
}

//returns time in seconds

int Clock::GetTime() 
{
	RTC_GetTime(RTC_Format_BCD, &m_RTCTimeStructure);
	return m_RTCTimeStructure.RTC_Seconds + m_RTCTimeStructure.RTC_Minutes* 60 + m_RTCTimeStructure.RTC_Hours * 3600;
}
