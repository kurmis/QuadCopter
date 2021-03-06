#ifndef __MEMS_H
#define __MEMS_H

#define ABS(x)         (x < 0) ? (-x) : x

#define L3G_Sensitivity_250dps     (float)   114.285f         /*!< gyroscope sensitivity with 250 dps full scale [LSB/dps] */
#define L3G_Sensitivity_500dps     (float)    57.1429f        /*!< gyroscope sensitivity with 500 dps full scale [LSB/dps] */
#define L3G_Sensitivity_2000dps    (float)    14.285f	      /*!< gyroscope sensitivity with 2000 dps full scale [LSB/dps] */
#define PI                         (float)     3.14159265f

#define LSM_Acc_Sensitivity_2g     (float)     1.0f            /*!< accelerometer sensitivity with 2 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_4g     (float)     0.5f            /*!< accelerometer sensitivity with 4 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_8g     (float)     0.25f           /*!< accelerometer sensitivity with 8 g full scale [LSB/mg] */
#define LSM_Acc_Sensitivity_16g    (float)     0.0834f         /*!< accelerometer sensitivity with 12 g full scale [LSB/mg] */

class Gyro
{
private:
	float m_offset[3];
  
public:
	Gyro(int init);
	void GetData(float* pfData);
  void Calibrate(int avgTimes, int delay);
};

class Compass
{

public:
	Compass(int init);
	void GetAcc(float* pfData);
	void GetMag(float* pfData);
};

class Sensors
{
private:
	float m_angles[3];
	Gyro* m_gyro;
	Compass* m_compass;
  bool m_print;
  
public:
	Sensors(Gyro* gyro, Compass* compass);
	void GetAngles(float* pfData);
	void CalcAngles(int timeOfLastMeasure);
  int GetPrint() { m_print = !m_print; return m_print; }
  void Calibrate(int avgTimes, int delay) { m_gyro->Calibrate(avgTimes, delay); }
};

#endif
