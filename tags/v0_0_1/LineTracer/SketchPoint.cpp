#include "StdAfx.h"
#include ".\sketchpoint.h"

CSketchPoint::CSketchPoint(int x, int y, bool isEndPoint, bool isKnot)
: CPoint(x,y)
, m_Width(0)
{
	m_isEndPoint=isEndPoint;
	m_isKnot=isKnot;
}

CSketchPoint::CSketchPoint(CPoint p, bool isEndPoint, bool isKnot)
: CPoint(p)
, m_Width(0)
{
	m_isEndPoint=isEndPoint;
	m_isKnot=isKnot;
}

CSketchPoint::~CSketchPoint(void)
{
}


bool CSketchPoint::IsKnot(void)
{
	return m_isKnot;
}

bool CSketchPoint::IsEndPoint(void)
{
	return m_isEndPoint;
}

void CSketchPoint::SetIsEndPoint(bool val)
{
	m_isEndPoint=val;
}

void CSketchPoint::SetIsKnot(bool val)
{
	m_isKnot=val;
}
