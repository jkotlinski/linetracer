#include "StdAfx.h"
#include ".\polyline.h"

CPolyLine::CPolyLine(void)
{
}

CPolyLine::~CPolyLine(void)
{
}

void CPolyLine::Add(CSketchPoint point)
{
	//TRACE("added point %i:%i\n",point.x,point.y);
	m_points.push_back(point);
}


bool CPolyLine::Contains(CPoint p)
{
	for(unsigned int i=0; i<m_points.size(); i++) {
		if(m_points.at(i)==p) {
			return true;
		}
	}
	return false;
}

int CPolyLine::Size(void)
{
	return (int)m_points.size();
}

CSketchPoint CPolyLine::At(int i)
{
	return m_points.at(i);
}

//return true if it has less than two knot points
bool CPolyLine::IsTail(void)
{
	vector<CSketchPoint>::const_iterator iter;

	int knotCount=0;
	for(iter=m_points.begin(); iter!=m_points.end(); iter++) {
		CSketchPoint point = *iter;
		if(point.IsKnot()) {
			knotCount++;
		}
	}

	return (knotCount<2)?false:true;
}
