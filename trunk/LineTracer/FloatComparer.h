#pragma once

class CFloatComparer
{
public:
	static bool FloatsDiffer(double a_val1, double a_val2);
private:
	static double Abs(double a_val);
};
