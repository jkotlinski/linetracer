#include "StdAfx.h"
#include ".\sketchpoint.h"

CSketchPoint::CSketchPoint(double x, double y, bool isEndPoint, bool isKnot)
: CFPoint(x,y)
, m_Width(0)
, m_isKnee(false)
, m_controlPointBack(CFPoint(x,y))
, m_controlPointForward(CFPoint(x,y))
, m_color(0)
, m_yFork(false)
{
	m_isEndPoint=isEndPoint;
	m_isKnot=isKnot;
}

CSketchPoint::CSketchPoint(CFPoint p, bool isEndPoint, bool isKnot)
: CFPoint(p)
, m_controlPointBack(p)
, m_controlPointForward(p)
, m_Width(0)
, m_isKnee(false)
, m_color(0)
, m_yFork(false)
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

CSketchPoint* CSketchPoint::Clone(void)
{
	CSketchPoint *nuPt = new CSketchPoint(x,y,IsEndPoint(),IsKnot());
	nuPt->SetControlPointBack(GetControlPointBack());
	nuPt->SetControlPointForward(GetControlPointForward());
	nuPt->SetKnee(IsKnee());
	nuPt->m_yFork=m_yFork;
	return nuPt;
}

void CSketchPoint::SetKnee(bool val)
{
	m_isKnee = val;
}

bool CSketchPoint::IsKnee(void)
{
	return m_isKnee;
}

void CSketchPoint::SetControlPointBack(CFPoint point)
{
	m_controlPointBack = point;
}

void CSketchPoint::SetControlPointForward(CFPoint point)
{
	m_controlPointForward = point;
}

CFPoint CSketchPoint::GetControlPointBack()
{
	return m_controlPointBack;
}

CFPoint CSketchPoint::GetControlPointForward()
{
	return m_controlPointForward;
}

void CSketchPoint::SwapControlPoints(void)
{
	CFPoint tmp = m_controlPointForward;
	m_controlPointForward = m_controlPointBack;
	m_controlPointBack = tmp;
}

void CSketchPoint::Trace(void)
{
	TRACE( "x=%f, y=%f\n", x, y );
}
