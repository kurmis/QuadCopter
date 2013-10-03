
#ifndef __UTILS_H
#define __UTILS_H
#include "stm32f30x.h"
#include "main.h"

void Delay(__IO uint32_t nTime);
uint8_t strlen(char* str);
void add(float* result, float* a, float* b);
void diff(float* result, float* a, float* b);

//multiply a vector by a constant
void mult(float* result, float* a, float b);

//dot product
float mult(float* a, float* b);
//cross product
void mult(float* result, float* a, float* b);

class Sleeper
{
private:
	unsigned int m_from;
public:
	void FromNow();
  void For(unsigned int ms);
};

void Isort(float *a, int n);
float FindMedian(float *data, int arraySize);

float Max(float a, float b);
float Min(float a, float b);
float Constrain(float c, float min, float max);


#endif
