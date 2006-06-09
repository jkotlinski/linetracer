#include "StdAfx.h"
#include "floatcomparer.h"

bool CFloatComparer::FloatsDiffer(double a_val1, double a_val2) {
	static const double K_LARGEST_DIFF_FOR_EQUALITY = 0.1;
	double l_diff = Abs ( a_val1 - a_val2 );
	if ( l_diff > K_LARGEST_DIFF_FOR_EQUALITY ) {
		return true;
	}
	return false;
}

double CFloatComparer::Abs(double a_val)
{
	return ( a_val < 0 ) ? -a_val : a_val;
}
