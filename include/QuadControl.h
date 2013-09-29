#ifndef __QUADCONTROL_H
#define __QUADCONTROL_H
#include "Mems.h"
#include "MotorControl.h"

extern volatile unsigned int msTicks;


struct PID
{
	float Kp;
	float Ki;
	float Kd;
	unsigned int dt;
	unsigned int oldTicks;
	float integral;
	float derivative;
	float oldError;
};

class QuadControl
{
private:
	Sensors* m_sensors;
  Motors* m_motors;
  PID m_pidX; 
  bool m_print;
  void InitPid(PID& pid, float p, float i, float d);
  float BalanceX(float delta);
  float CalcPID(PID& pid, float error);
  
public:
	QuadControl(Sensors* sensors, Motors* motors);
	void Balance(float* angle);
  void SetPIDX(float p, float i, float d);
  void TogglePrint() { m_print = !m_print; }
  bool GetPrint() { return m_print; }
  void PrintPIDVals();
};



#endif
