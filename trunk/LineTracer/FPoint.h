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

#pragma once

class CFPoint
{
public:
	explicit CFPoint(double ix=0,double iy=0);
	virtual ~CFPoint(void);
	const CFPoint operator-(void) const;
	const CFPoint Unit(void) const;
	const CFPoint operator+=(const CFPoint &point);
	const double Distance(const CFPoint &point) const;
	const double GetX(void) const;
	const double GetY(void) const;
	void SetX(const double a_val);
	void SetY(const double a_val);
	const unsigned int HashValue() const;
private:
	double m_x;
	double m_y;
public:
	PointF GetPointF(void);
};

bool operator<(const CFPoint& p1,const CFPoint& p2);
const CFPoint operator*(const CFPoint& point, const double val);
const CFPoint operator/(const CFPoint& point, const double val);
double operator*(const CFPoint& point1, const CFPoint& point2);
const CFPoint operator-(const CFPoint& original, const CFPoint& subtractor);
const CFPoint operator+(const CFPoint& point1, const CFPoint& point2);
const bool operator==(const CFPoint& a_p1, const CFPoint& a_p2);
const bool operator!=(const CFPoint& a_p1, const CFPoint& a_p2);

inline const double CFPoint::GetX(void) const {
	return m_x;
}
inline const double CFPoint::GetY(void) const {
	return m_y;
}
