#pragma once

#include <vector>

class Magnification
{
public:
	Magnification(void);
	~Magnification(void);
	void Increase(void);
	void Decrease(void);
private:

	unsigned int m_level;
public:
	double GetValue(void);
private:
	std::vector<double> m_levelSteps;
};
