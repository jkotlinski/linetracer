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
	CFPoint m_controlPointBack;
	CFPoint m_controlPointForward;
	double m_Width;
	int m_color;
public:
	const bool IsKnee(void) const;
	void SetControlPointBack(const CFPoint &point);
	void SetControlPointForward(const CFPoint &point);
	CFPoint GetControlPointBack() const;
	CFPoint GetControlPointForward() const;
	void SwapControlPoints(void);
	void Trace(void) const;
public:
	const double GetX(void) const;
	const double GetY(void) const;
	const CFPoint GetCoords(void) const;
	const double Distance(const CFPoint &a_p) const;
	void SetX(double a_val);
	void SetY(double a_val);
	void SetIsYFork(bool a_isTrue);
	const bool IsYFork(void) const;
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
