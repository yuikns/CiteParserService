#include "myMath.h"
#include <math.h>

double getVariance(int list[],int len)
{
	int sum = 0;
	double avg;
	double vari = 0;
	for(int i=0;i<len;i++) sum += list[i];
	avg = (double)sum / len;
	for(int i=0;i<len;i++) vari += (list[i]-avg)*(list[i]-avg);
	return vari;
}

double getAverage(int list[],int len)
{
	int sum = 0;
	for(int i=0;i<len;i++) sum += list[i];
	return  (double)sum / len;;
}

