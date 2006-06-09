#pragma once

#include <vector>

class Magnification
{
public:
	Magnification(void);
	~Magnification(void);
	void Increase(void);
	bool Decrease(void);

private:
	const unsigned int m_defaultLevel;
	unsigned int m_level;
public:
	double GetValue(void);
private:
	std::vector<double> m_levelSteps;
public:
	void Reset(void);
};
