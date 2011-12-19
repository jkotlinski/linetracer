/** This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "StdAfx.h"
#include "fpoint.h"

#include <math.h>

CFPoint::CFPoint(double ix,double iy)
: m_x(ix)
, m_y(iy)
{
}

CFPoint::~CFPoint(void)
{
}

const CFPoint CFPoint::operator-(void) const
{	
	return CFPoint(-m_x,-m_y);
}

const CFPoint CFPoint::Unit(void) const
{
	double scale = sqrt(m_x*m_x + m_y*m_y);
	return CFPoint(m_x/scale, m_y/scale);
}

const CFPoint CFPoint::operator+=(const CFPoint & point)
{
	m_x += point.GetX();
	m_y += point.GetY();
	return CFPoint(m_x,m_y);
}

const double CFPoint::Distance(const CFPoint &point) const
{
	const double xdiff = point.GetX() - m_x;
	const double ydiff = point.GetY() - m_y;

	return sqrt(xdiff*xdiff+ydiff*ydiff);
}

// -----------------------------------------------------
// non-member variables

const CFPoint operator*(const CFPoint& point, const double val)
{
	return CFPoint(point.GetX()*val,point.GetY()*val);
}
const CFPoint operator/(const CFPoint& point, const double val)
{
	return CFPoint(point.GetX()/val,point.GetY()/val);
}
double operator*(const CFPoint& point1, const CFPoint& point2)
{
	return point1.GetX() * point2.GetX() + point1.GetY() * point2.GetY();
}

const CFPoint operator-(const CFPoint& original, const CFPoint& subtractor)
{
	return CFPoint( original.GetX() - subtractor.GetX(),
		original.GetY() - subtractor.GetY() );
}

bool operator<(const CFPoint& p1,const CFPoint& p2)
{
	if(p1.GetX() < p2.GetX()) return true;
	if(p1.GetX() > p2.GetX()) return false;
	if(p1.GetY() < p2.GetY()) return true;
	return false;
}

const bool operator==(const CFPoint& a_p1, const CFPoint& a_p2)
{
	static const double MAX_DIFF_FOR_EQUALITY = 0.71;
	return a_p1.Distance(a_p2) < MAX_DIFF_FOR_EQUALITY;
}

const bool operator!=(const CFPoint& a_p1, const CFPoint& a_p2)
{
	return !(a_p1 == a_p2);
}

const CFPoint operator+(const CFPoint& a_p1, const CFPoint& a_p2)
{
	return CFPoint(a_p1.GetX() + a_p2.GetX(), a_p1.GetY() + a_p2.GetY());
}

void CFPoint::SetX(const double a_val) {
	m_x = a_val;
}

void CFPoint::SetY(const double a_val) {
	m_y = a_val;
}

const unsigned int CFPoint::HashValue() const {
	unsigned int l_x = static_cast<unsigned int> ( GetX() + 0.5 );
	unsigned int l_y = static_cast<unsigned int> ( GetY() + 0.5 );
	return ( l_x << 16 ) | l_y;
}

PointF CFPoint::GetPointF(void)
{
	return PointF( static_cast<REAL>(m_x),
		static_cast<REAL>(m_y) );
}
