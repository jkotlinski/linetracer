#pragma once
#include "atltypes.h"

#include "FPoint.h"

class CSketchPoint :
	public CFPoint
{
public:
	~CSketchPoint(void);
protected:
	bool m_isEndPoint;
public:
	CSketchPoint(double x,double y,bool isEndPoint=false,bool isKnot=false);
	CSketchPoint(CFPoint p,bool isEndPoint=false,bool isKnot=false);
protected:
	bool m_isKnot;
	float m_Width;
public:
	bool IsKnot(void);
	bool IsEndPoint(void);
	void SetIsEndPoint(bool val);
	void SetIsKnot(bool val);
	CSketchPoint* Clone(void);
	void SetKnee(bool val);
private:
	bool m_isKnee;
public:
	bool IsKnee(void);
private:
	CFPoint m_controlPointBack;
	CFPoint m_controlPointForward;
public:
	void SetControlPointBack(CFPoint point);
	void SetControlPointForward(CFPoint point);
	CFPoint GetControlPointBack();
	CFPoint GetControlPointForward();
	int m_color;
	void SwapControlPoints(void);
	bool m_yFork;
};
