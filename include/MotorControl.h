#ifndef __MOTOR_CONTROL_H
#define __MOTOR_CONTROL_H
#define MOTOR_LOW 0.45f
#define MOTOR_HIGH 0.9f
#define INIT_PWM 400
#define LED_NUM 8

class Motor
{
public:
	//do not initialize directly
	Motor(int motor,int period);
	void SetSpeed(float level);
  float GetSpeed();
	
private:
	int m_pwmPeriod;
	int m_motor;
	float m_level;
	void SetPwmWidthNorm(int channel, int pwm_period, float duty_cycle);
	void SetPwmWidth(int channel, int pwm_period, int duty_cycle);
	
};

class Motors {

public:
	Motor* motor1;
	Motor* motor2;
	Motor* motor3;
	Motor* motor4;
	
public:
	Motors(bool initHigh);
	void SetSpeed(float level);
	void SetSpeed(int motor, float level);
	void Reinit();
	int GetPwmPeriod();

private:
	
	void InitPwmGpio(void);
	void SetPwmWidthNorm(int channel, int pwm_period, float duty_cycle);
	void SetPwmWidth(int channel, int pwm_period, int duty_cycle);
	int InitPwm(int pwm_freq);	
	int m_pwmPeriod;

};

#endif
