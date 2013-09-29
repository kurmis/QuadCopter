#include "QuadControl.h"
#include "Utils.h"
#include "stdio.h"

float MIN_SPEED = 50;

QuadControl::QuadControl(Sensors* sensors, Motors* motors)
{
	m_sensors = sensors;
	m_motors = motors;
	InitPid(m_pidX, 1, 0, 0);
	m_print = false;
}
void QuadControl::InitPid(PID& pid, float p, float i, float d)
{
	pid.Kp = p;
	pid.Ki = i;
	pid.Kd = d;
  pid.dt = 0;;
	pid.oldTicks = 0;
	pid.integral = 0;
	pid.derivative = 0;
	pid.oldError = 0;  
}

void QuadControl::SetPIDX(float p, float i, float d)
{
	InitPid(m_pidX, p, i, d);
}

void QuadControl::Balance(float* angle)
{
	float curAngle[3] = {0};
  m_sensors->GetAngles(curAngle);
	float delta[3];
	diff(delta, angle, curAngle);
	float deltaX = BalanceX(delta[0]);
  float m1speed = m_motors->motor1->GetSpeed();
  float m2speed = m_motors->motor2->GetSpeed();
  m1speed += deltaX/2.0f;
  m2speed -= deltaX/2.0f;
  m_motors->motor1->SetSpeed(m1speed);
  m_motors->motor2->SetSpeed(m2speed);
  if(MIN_SPEED > m1speed)
	{
		m_motors->motor1->SetSpeed(MIN_SPEED);
	}
	if(MIN_SPEED > m2speed)
	{
		m_motors->motor2->SetSpeed(MIN_SPEED);
	}
  
  if(m_print)
	{
		static int i = 0;
		i++;
		if( i > 70)
		{
			i = 0;
			printf("pidX output = '%g'\r\n", deltaX);
			printf("motor1 speed set = '%g'\r\n", m_motors->motor1->GetSpeed());
			printf("motor2 speed set = '%g'\r\n", m_motors->motor2->GetSpeed());
		}
	}
}

float QuadControl::BalanceX(float delta)
{
	return CalcPID(m_pidX, delta);
}

/*error = setpoint - measured_value
  integral = integral + error*dt
  derivative = (error - previous_error)/dt
  output = Kp*error + Ki*integral + Kd*derivative
  previous_error = error
*/
float QuadControl::CalcPID(PID& pid, float error)
{
	pid.integral += error*(pid.oldTicks-msTicks)/1000.0f;
	pid.derivative = (error - pid.oldError)*1000.0f/(pid.oldTicks-msTicks);
	pid.oldError = error;
	pid.oldTicks = msTicks;
	return pid.Kp*error + pid.Ki*pid.integral + pid.Kd*pid.derivative;
}

void QuadControl::PrintPIDVals()
{
	printf("PIDX: Kp = %g, Ki = %g, Kd = %g\n\r", m_pidX.Kp, m_pidX.Ki, m_pidX.Kd);
}