#include "StdAfx.h"
#include ".\fpoint.h"

#include <math.h>

CFPoint::CFPoint(double ix,double iy)
: x(ix)
, y(iy)
{
}

CFPoint::~CFPoint(void)
{
}

bool CFPoint::operator==(const CFPoint & point)
{
	if(point.x == x && point.y == y) return true;
	return false;
}

bool CFPoint::operator!=(const CFPoint& point)
{
	if(point.x != x || point.y != y) return true;
	return false;
}

CFPoint CFPoint::operator*(const double val)
{
	return CFPoint(x*val,y*val);
}

CFPoint CFPoint::operator-(void)
{
	return CFPoint(-x,-y);
}

CFPoint CFPoint::operator+(const CFPoint& point)
{
	return CFPoint(x+point.x,y+point.y);
}

CFPoint CFPoint::operator-(const CFPoint& point)
{
	return CFPoint(x-point.x,y-point.y);
}

double CFPoint::operator*(const CFPoint& point)
{
	return x*point.x+y*point.y;
}

bool operator<(const CFPoint& p1,const CFPoint& p2)
{
	if(p1.x < p2.x) return true;
	if(p1.x > p2.x) return false;
	if(p1.y < p2.y) return true;
	return false;
}

CFPoint CFPoint::Unit(void)
{
	double scale = sqrt(x*x+y*y);
	return CFPoint(x/scale,y/scale);
}

CFPoint CFPoint::operator+=(const CFPoint & point)
{
	x += point.x;
	y += point.y;
	return CFPoint(x,y);
}

double CFPoint::Distance(const CFPoint point)
{
	double xdiff = point.x - x;
	double ydiff = point.y - y;

	return sqrt(xdiff*xdiff+ydiff*ydiff);
}
