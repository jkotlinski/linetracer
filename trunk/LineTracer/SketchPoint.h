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
#include "atltypes.h"

#include "FPoint.h"

class CSketchPoint
{
public:
	~CSketchPoint(void);
public:
	CSketchPoint();
	explicit CSketchPoint(double a_x, double a_y,bool isEndPoint=false,bool isKnot=false);
	explicit CSketchPoint(const CFPoint &p, bool isEndPoint=false, bool isKnot=false);
public:
	const bool IsKnot(void) const;
	const bool IsEndPoint(void) const;
	void SetIsEndPoint(bool val);
	void SetIsKnot(bool val);
	CSketchPoint* Clone(void) const;
	void SetKnee(bool val);
private:
	CFPoint m_point;
	bool m_isEndPoint;
	bool m_isKnot;
	bool m_isKnee;
	bool m_isYFork;
	CFPoint m_controlPoint;
	double m_Width;
	int m_color;
public:
	const bool IsKnee(void) const;
	void SetControlPoint(const CFPoint &point);
	CFPoint GetControlPoint() const;
	void Trace(void) const;
public:
	const double GetX(void) const;
	const double GetY(void) const;
	const CFPoint GetCoords(void) const;
	void SetCoords(CFPoint p);
	const double Distance(const CFPoint &a_p) const;
	void SetX(double a_val);
	void SetY(double a_val);
	void SetLineWidthGreaterThanMedianOfLine(bool a_isTrue);
	const bool IsLineWidthGreaterThanMedianOfLine(void) const;
	PointF GetPointF(void);
	float Distance(const CSketchPoint & a_point) const;
private:
	CFPoint GetCFPoint(void) const;
};

inline const double CSketchPoint::GetX(void) const {
	return m_point.GetX();
}

inline const double CSketchPoint::GetY(void) const {
	return m_point.GetY();
}

inline const CFPoint CSketchPoint::GetCoords(void) const {
	return m_point;
}
