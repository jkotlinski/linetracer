#include "StdAfx.h"
#include "sketchpoint.h"

#include "Logger.h"
#include ".\sketchpoint.h"

CSketchPoint::CSketchPoint(double a_x, double a_y, bool isEndPoint, bool isKnot)
: m_point(a_x,a_y)
, m_isEndPoint(isEndPoint)
, m_isKnot(isKnot)
, m_isKnee(false)
, m_isYFork(false)
, m_controlPoint(CFPoint(a_x,a_y))
, m_Width(0)
, m_color(0)
{
}

CSketchPoint::CSketchPoint(const CFPoint &p, bool isEndPoint, bool isKnot)
: m_point(p)
, m_isEndPoint(isEndPoint)
, m_isKnot(isKnot)
, m_isKnee(false)
, m_isYFork(false)
, m_controlPoint(p)
, m_Width(0)
, m_color(0)
{
}

CSketchPoint::CSketchPoint()
: m_point()
, m_isEndPoint(false)
, m_isKnot(false)
, m_isKnee(false)
, m_isYFork(false)
, m_controlPoint()
, m_Width(0)
, m_color(0)
{
}

CSketchPoint::~CSketchPoint(void)
{
}

const bool CSketchPoint::IsKnot(void) const
{
	return m_isKnot;
}

const bool CSketchPoint::IsEndPoint(void) const
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

CSketchPoint* CSketchPoint::Clone(void) const {
	CSketchPoint *nuPt = new CSketchPoint(m_point.GetX(),m_point.GetY(),IsEndPoint(),IsKnot());
	nuPt->SetControlPoint(GetControlPoint());
	nuPt->SetKnee(IsKnee());
	nuPt->m_isYFork=m_isYFork;
	return nuPt;
}

void CSketchPoint::SetKnee(bool val)
{
	m_isKnee = val;
}

const bool CSketchPoint::IsKnee(void) const {
	return m_isKnee;
}

void CSketchPoint::SetControlPoint(const CFPoint &point) {
	m_controlPoint = point;
}

CFPoint CSketchPoint::GetControlPoint() const {
	return m_controlPoint;
}

void CSketchPoint::Trace(void) const {
	TRACE ( "x=%f, y=%f\n", m_point.GetX(), m_point.GetY() );
}


const double CSketchPoint::Distance(const CFPoint & a_p) const
{
	return m_point.Distance(a_p);
}

void CSketchPoint::SetX(double a_val)
{
	m_point.SetX(a_val);
}

void CSketchPoint::SetY(double a_val)
{
	m_point.SetY(a_val);
}

void CSketchPoint::SetLineWidthGreaterThanMedianOfLine(bool a_isTrue)
{
	m_isYFork = a_isTrue;
}

const bool CSketchPoint::IsLineWidthGreaterThanMedianOfLine(void) const {
	return m_isYFork;
}

PointF CSketchPoint::GetPointF(void)
{
	return m_point.GetPointF();
}

float CSketchPoint::Distance(const CSketchPoint & a_point) const
{
	return float(m_point.Distance( a_point.GetCFPoint() ));
}

CFPoint CSketchPoint::GetCFPoint(void) const
{
	return m_point;
}

void CSketchPoint::SetCoords(CFPoint p)
{
	m_point = p;
}