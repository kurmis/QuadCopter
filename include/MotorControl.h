#ifndef __MOTOR_CONTROL_H
#define __MOTOR_CONTROL_H
#define MOTOR_LOW 0.45f
#define MOTOR_HIGH 0.82f
#define LED_NUM 8

static int _pwm_period;

typedef enum { false, true } bool;
extern void init_motors(bool initHigh);
extern void setMotorSpeed(int motor, float level);

void init_pwm_gpio(void);
void set_pwm_width_norm(int channel, int pwm_period, float duty_cycle);
void set_pwm_width(int channel, int pwm_period, int duty_cycle);
int init_pwm(int pwm_freq);

#endif
