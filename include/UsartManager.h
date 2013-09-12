#ifndef __USARTMANAGER_H
#define __USARTMANAGER_H
#include "MotorControl.h"
#include "Mems.h"


class UsartManager
{
public:
	
	UsartManager(int inten, Motors* motors, Sensors* sensors);
	int GetCommand(volatile char * buffer, int size);

private:
	Motors* m_motors;
	Sensors* m_sensors;
	int Identify(volatile char* buffer, int size, int start, int stop);
	void Clear_Buffer(volatile char* buffer, int size);
	int compStr (char *s1, char *s2, int sz);
	void Init_Commands(void);
};

#endif
