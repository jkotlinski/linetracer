#pragma once
#include "atltypes.h"

class CSketchPoint :
	public CPoint
{
public:
	~CSketchPoint(void);
protected:
	bool m_isEndPoint;
public:
	CSketchPoint(int x,int y,bool isEndPoint=false,bool isKnot=false);
	CSketchPoint(CPoint p,bool isEndPoint=false,bool isKnot=false);
protected:
	bool m_isKnot;
	float m_Width;
public:
	bool IsKnot(void);
	bool IsEndPoint(void);
	void SetIsEndPoint(bool val);
	void SetIsKnot(bool val);
};
