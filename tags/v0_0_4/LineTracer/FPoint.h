#pragma once

class CFPoint
{
public:
	CFPoint(double ix=0,double iy=0);
	~CFPoint(void);
	double x;
	double y;
	bool operator==(const CFPoint & point);
	bool operator!=(const CFPoint& point);
	CFPoint operator*(const double val);
	CFPoint operator-(void);
	CFPoint operator+(const CFPoint& point);
	CFPoint operator-(const CFPoint& point);
	double operator*(const CFPoint& point);
	CFPoint Unit(void);
	CFPoint operator+=(const CFPoint & point);
	double Distance(const CFPoint point);
};

bool operator<(const CFPoint& p1,const CFPoint& p2);