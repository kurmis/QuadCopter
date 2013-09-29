#include "utils.h"

extern volatile unsigned int msTicks;

void Delay(__IO uint32_t nTime)
{
	uint32_t delta = msTicks + nTime;
	while(delta > msTicks);
}

uint8_t strlen(char* str)
{
	uint8_t i;
	for(i = 0; str[i]; i++)
		;
	return i;
}

void add(float* result, float* a, float* b)
{
	for(int i = 0; i < 3; i++)
	{
		result[i] = a[i] + b[i];
	}
}
void diff(float* result, float* from, float* to)
{
	for(int i = 0; i < 3; i++)
	{
		result[i] = from[i] - to[i];
	}
}

//multiply a vector by a constant
void mult(float* result, float* a, float b)
{
	for(int i = 0; i < 3; i++)
	{
		result[i] = a[i] * b;
	}
}
//dot product 
float mult(float* a, float* b)
{
	float sum = 0;
	for(int i = 0; i < 3; i++)
	{
		sum += a[i] * b[i];
	}
	return sum;
}

//cross product
void mult(float* result, float* a, float* b)
{
	result[0] = a[2]*b[3] - a[3]*b[2];
	result[1] = a[3]*b[1] - a[1]*b[3];
	result[2] = a[1]*b[2] - a[2]*b[1];

}

