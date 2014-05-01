#include "MotorControl.h"
#include "utils.h"
#include "stdio.h"

int Motors::GetPwmPeriod()
{
	return m_pwmPeriod;
}
Motors::Motors(bool initHigh)
{
	InitPwmGpio();
	m_pwmPeriod = InitPwm(INIT_PWM);
	if(initHigh)
	{
		Reinit();
	}
}

void Motors::SetSpeed(float level)
{
	motor1->SetSpeed(level);
	motor2->SetSpeed(level);
	motor3->SetSpeed(level);
	motor4->SetSpeed(level);	
}
void Motors::SetSpeed(int motor, float level)
{
	switch (motor){
		case 1: motor1->SetSpeed(level); break;
		case 2: motor2->SetSpeed(level); break;
		case 3: motor3->SetSpeed(level); break;
		case 4: motor4->SetSpeed(level); break;
		default: printf("Invalid motor %d\n\r", motor); break;
	}
}
/*
motor = [1..4]
level = [0..1000]

*/
void Motors::Reinit()
{
	SetSpeed(1000);
	Delay(2000);
	SetSpeed(0);
	Delay(4000);
}
Motor::Motor(int motor,int period)
{
	m_motor = motor;
	m_pwmPeriod = period;
	SetPwmWidthNorm(m_motor, m_pwmPeriod, 0.0f);
}

void Motor::SetSpeed(float level)
{
	float duty_cycle= 0.0f;
	m_level = level;
	if(level < 1)
	{
		m_level = 1.0f;
	}
	else if (level > 1000)
	{
		m_level = 1000.0f;
	}
	
	duty_cycle = MOTOR_LOW + m_level * (MOTOR_HIGH-MOTOR_LOW) / 1000.0f; 
	SetPwmWidthNorm(m_motor, m_pwmPeriod, duty_cycle);
	
}
float Motor::GetSpeed()
{
	return m_level;
}
void Motors::InitPwmGpio(void)
{
	// The Timer 1 channels 1,2 and 3 are connected to the LED pins on the Discovery board
	// To drive the ECSs they could to be connected to other pins if needed.

	// Setup the LEDs
	// Check out the Discovery schematics
	// http://www.st.com/st-web-ui/static/active/en/resource/technical/document/user_manual/DM00063382.pdf
		//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; // Use the alternative pin functions
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // GPIO speed - has nothing to do with the timer timing
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // Push-pull
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; // Setup pull-up resistors
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	// Connect the timer output to the LED pins
	// Check the alternative function mapping in the CPU doc
	// http://www.st.com/st-web-ui/static/active/en/resource/technical/document/datasheet/DM00058181.pdf
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_2); // TIM4_CH1 -> LED3
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_2); // TIM4_CH3 -> LED10
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_2); // TIM4_CH2 -> LED7
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_2);
	
}

int Motors::InitPwm(int pwm_freq)
{
	
	TIM_OCInitTypeDef  TIM_OCInitStructure_pwm;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	// Calculates the timing. This is common for all channels
	int clk = 72e6; // 72MHz -> system core clock. This is default on the stm32f3 discovery
	int tim_freq = 2e6; // in Hz (2MHz) Base frequency of the pwm timer
	int prescaler = ((clk / tim_freq) - 1);

	// Calculate the period for a given pwm frequency
	//		int pwm_freq = 200; // in Hz
	int pwm_period = tim_freq/pwm_freq;		// 2MHz / 200Hz = 10000
												// For 50Hz we get: 2MHz / 50Hz = 40000

	// Calculate a number of pulses per millisecond.
	// Not used in this rutine but I put it here just as an example
	int ms_pulses = (float)pwm_period / (1000.0/pwm_freq); // for 200Hz we get: 10000 / (1/200 * 1000) = 2000


//  Enable the TIM4 peripherie
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE );

// Setup the timing and configure the TIM4 timer
	
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
	TIM_TimeBaseStructure.TIM_Period = pwm_period - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up ;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);


// Initialise the timer channels
	

	TIM_OCInitStructure_pwm.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure_pwm.TIM_OutputState = TIM_OutputState_Enable;

	TIM_OCInitStructure_pwm.TIM_Pulse = ms_pulses*2; // preset pulse width 0..pwm_period
	TIM_OCInitStructure_pwm.TIM_OCPolarity = TIM_OCPolarity_High; // Pulse polarity
	//	  TIM_OCInitStructure_pwm_pwm.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure_pwm.TIM_OCIdleState = TIM_OCIdleState_Set;

	// These settings must be applied on the timer 1.
	TIM_OCInitStructure_pwm.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure_pwm.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure_pwm.TIM_OCNIdleState = TIM_OCIdleState_Set;

// Setup four channels
	// Channel 1
	TIM_OC1Init(TIM4, &TIM_OCInitStructure_pwm);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

	// Channel 2
	TIM_OC2Init(TIM4, &TIM_OCInitStructure_pwm);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);

	// Channel 3
	TIM_OC3Init(TIM4, &TIM_OCInitStructure_pwm);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);

	// Channel 4
	TIM_OC4Init(TIM4, &TIM_OCInitStructure_pwm);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);

	// Starup the timer
	//TIM_ARRPreloadConfig(TIM4, DISABLE);
	TIM_ARRPreloadConfig(TIM4, DISABLE);
	TIM_CtrlPWMOutputs(TIM4, ENABLE);
	TIM_Cmd(TIM4 , ENABLE);

	// The PWM is running now. The pulse width can be set by
	// TIM4->CCR1 = [0..pwm_period] -> 0..100% duty cycle
	//
	// For example:
	// int pulse_width = 3000;
	// TIM4->CCR1 = pulse_width;
	//
	// The firmware offers a API to do this:
	// TIM_SetCompare1(TIM4 , pulse_width); // This is a wrapper for TIM4->CCR1, the same as TIM1->CCR1=pulse_width;

	return pwm_period;
}

void Motor::SetPwmWidth(int channel, int pwm_period, int duty_cycle)
{
	int pwm_pulses = pwm_period*duty_cycle/100.0;
	switch (channel){
		case 1: TIM_SetCompare1(TIM4, pwm_pulses); break;
		case 2: TIM_SetCompare2(TIM4, pwm_pulses); break;
		case 3: TIM_SetCompare3(TIM4, pwm_pulses); break;
		case 4: TIM_SetCompare4(TIM4, pwm_pulses); break;
	}
}

/**
  * @brief  Sets the PWM duty cycle per channel
  * @param  channel:  PWM channel index [1..4]
  * @param  duty_cycle:  PWM duty cycle (float) [0..1]
  * @retval None
  */
void Motor::SetPwmWidthNorm(int channel, int pwm_period, float duty_cycle)
{
	int pwm_pulses = pwm_period*(float)duty_cycle;
	switch (channel){
		case 1: TIM_SetCompare1(TIM4, pwm_pulses); break;
		case 2: TIM_SetCompare2(TIM4, pwm_pulses); break;
		case 3: TIM_SetCompare3(TIM4, pwm_pulses); break;
		case 4: TIM_SetCompare4(TIM4, pwm_pulses); break;
	}
}

