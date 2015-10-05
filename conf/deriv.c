#include "deriv.h"

float deriv(float x0, float x1, int t0, int t1)
{
	int timeStamp = 0;
	float deriv = 0;

	if(t0 < t1)
	{
		timeStamp = t1 - t0;
		deriv = (x1 - x0)/timeStamp;
	}
	if(t0 > t1)
	{
		timeStamp = t0 - t1;
		deriv = (x0 - x1)/timeStamp;
	}

	return deriv;
}
