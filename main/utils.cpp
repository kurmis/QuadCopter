#include "utils.h"
#include <stdio.h>

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

void Sleeper::For(unsigned int ms)
{
	unsigned int timeToSleep = ms + m_from;
	if(timeToSleep < msTicks)
	{
		printf("Overslept by %dms\n\r", msTicks - timeToSleep);
	}
	while(msTicks < timeToSleep);
}
void Sleeper::FromNow() { m_from = msTicks; }
void Isort(float *a, int n){
  for (int i = 1; i < n; ++i){
    float j = a[i];
    int k;
    for (k = i - 1; (k >= 0) && (j < a[k]); k--){
      a[k + 1] = a[k];
    }
    a[k + 1] = j;
  }
}

float FindMedian(float *data, int arraySize){
  Isort(data, arraySize);
  
  return data[arraySize/2];
}

float Min(float a, float b)
{
	if(a > b)
		return b;
	return a;
}
float Max(float a, float b)
{
	if(a > b)
	{
		return a;
	}
	return b;
}
float Constrain(float c, float min, float max)
{
	return Max(min, Min(max, c));
}

