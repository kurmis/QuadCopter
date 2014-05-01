#include "QuadControl.h"
#include "Utils.h"
#include "stdio.h"

float MIN_SPEED = 100.0f;
float MAX_SPEED = 700.0f;

QuadControl::QuadControl(Sensors* sensors, Motors* motors)
{
	m_sensors = sensors;
	m_motors = motors;
	InitPid(m_pidX, 5.2, 0.0f, 0.008);
	InitPid(m_pidY, 5.2, 0.0f, 0.008);
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
void QuadControl::SetPIDY(float p, float i, float d)
{
	InitPid(m_pidY, p, i, d);
}

void QuadControl::Balance(float* angle)
{
	float curAngle[3] = {0};
  m_sensors->GetAngles(curAngle);
	float delta[3] = {0}; 
	diff(delta, angle, curAngle);
	float deltaX = BalanceX(delta[0]);
  float deltaY = BalanceY(delta[1]);
  float m1speed = m_throttle;
  float m2speed = m_throttle;
  float m3speed = m_throttle;
  float m4speed = m_throttle;
  m1speed += deltaX/2.0f;
  m2speed += deltaY/2.0f;
  m3speed -= deltaX/2.0f;
  m4speed -= deltaY/2.0f;
	m_motors->motor4->SetSpeed(Constrain(m4speed,MIN_SPEED, MAX_SPEED));
  m_motors->motor1->SetSpeed(Constrain(m1speed,MIN_SPEED, MAX_SPEED));
  m_motors->motor2->SetSpeed(Constrain(m2speed,MIN_SPEED, MAX_SPEED));
  m_motors->motor3->SetSpeed(Constrain(m3speed,MIN_SPEED, MAX_SPEED));
  
  
  if(m_print)
	{
		static int i = 0;
		i++;
		if( i > 30)
		{
			i = 0;
			printf("pidX output = '%g'\r\n", deltaX);
			printf("motor1 speed set = '%g'\r\n", m_motors->motor1->GetSpeed());
			printf("motor2 speed set = '%g'\r\n", m_motors->motor2->GetSpeed());
			printf("motor3 speed set = '%g'\r\n", m_motors->motor3->GetSpeed());
			printf("motor4 speed set = '%g'\r\n", m_motors->motor4->GetSpeed());
		}
	}
}

float QuadControl::BalanceX(float delta)
{
	return CalcPID(m_pidX, delta);
}
float QuadControl::BalanceY(float delta)
{
	return CalcPID(m_pidY, delta);
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
	printf("PIDY: Kp = %g, Ki = %g, Kd = %g\n\r", m_pidY.Kp, m_pidY.Ki, m_pidY.Kd);
}
