#include "StdAfx.h"
#include "polyline.h"
#include "Logger.h"

#include <list>
#include <math.h>
#include <algorithm>

#include "Binarizer.h"
#include "polyline.h"
#include ".\polyline.h"

CPolyLine::CPolyLine(void)
: m_isTail(false)
{
}

CPolyLine::~CPolyLine(void)
{
	try {
		vector<CSketchPoint*>::iterator iter;
		for(iter=Begin(); iter!=End(); iter++) {
			if(*iter!=NULL) {
				delete *iter;
				*iter = NULL;
			}
		}
	} catch (...) {
		try {
			LOG ( "!!!! caught exception in ~CPolyLine" );
			ASSERT ( false );
		} catch (...) {
		}
	}
}

void CPolyLine::Add(CSketchPoint *point)
{
	//LOG("added point %i:%i\n",point.x,point.y);
	m_points.push_back(point);
}


bool CPolyLine::Contains(CPoint p)
{
	vector<CSketchPoint*>::iterator iter;
	for(iter=Begin(); iter!=End(); iter++) {
		if((*iter)->GetX() == p.x && (*iter)->GetY() == p.y) {
			return true;
		}
	}
	return false;
}

unsigned int CPolyLine::Size(void)
{
	return (unsigned int) m_points.size();
}

/*CSketchPoint *CPolyLine::At(int i)
{
	return m_points.at(i);
}*/

CPolyLine* CPolyLine::Clone(void)
{
	CPolyLine *newLine = new CPolyLine();

	newLine->SetTail(IsTail());

	vector<CSketchPoint*>::iterator iter;

	for(iter = Begin(); iter != End(); iter++) {
		newLine->Add((*iter)->Clone());
	}
	return newLine;
}

int CPolyLine::HasKnots(void)
{
	int knotCount=0;

	if(GetHeadPoint()->IsKnot()) knotCount++;
	if(GetTailPoint()->IsKnot()) knotCount++;

	return knotCount;
}

CSketchPoint *CPolyLine::GetHeadPoint(void)
{
	return m_points.front();
}

CSketchPoint *CPolyLine::GetTailPoint(void)
{
	return m_points.back();
}

/* return a new line that is a merge of this and the other line.

the closest endpoints of the lines will be merged.
one of the endpoints that are glued together will be throwed 
away - but this shouldn't matter if those points are close.
*/
CPolyLine* CPolyLine::MergeLine(CPolyLine* line) 
{
	CLogger *l_logger = CLogger::Instance();
	l_logger->Inactivate();
	
	CPolyLine *tmp=new CPolyLine();

	enum {
		NO_INTERSECTION,
		INTERSECTION_HEAD_TO_HEAD,
		INTERSECTION_HEAD_TO_TAIL,
		INTERSECTION_TAIL_TO_HEAD,
		INTERSECTION_TAIL_TO_TAIL
	};
	
	double l_shortestIntersectionDistance = 10000; 
	int l_shortestIntersectionType = NO_INTERSECTION;
	
	//find best intersection type

	double l_distance = GetHeadPoint()->Distance( line->GetHeadPoint()->GetCoords() );
	if( l_distance < l_shortestIntersectionDistance )
	{
		l_shortestIntersectionDistance = l_distance;
		l_shortestIntersectionType = INTERSECTION_HEAD_TO_HEAD;
	}
	
	l_distance = GetHeadPoint()->Distance( line->GetTailPoint()->GetCoords() );
	if( l_distance < l_shortestIntersectionDistance )
	{
		l_shortestIntersectionDistance = l_distance;
		l_shortestIntersectionType = INTERSECTION_HEAD_TO_TAIL;
	}
	
	l_distance = GetTailPoint()->Distance( line->GetHeadPoint()->GetCoords() );
	if( l_distance < l_shortestIntersectionDistance )
	{
		l_shortestIntersectionDistance = l_distance;
		l_shortestIntersectionType = INTERSECTION_TAIL_TO_HEAD;
	}

	l_distance = GetTailPoint()->Distance( line->GetTailPoint()->GetCoords() );
	if( l_distance < l_shortestIntersectionDistance )
	{
		l_shortestIntersectionDistance = l_distance;
		l_shortestIntersectionType = INTERSECTION_TAIL_TO_TAIL;
	}

	switch( l_shortestIntersectionType )
	{
	case INTERSECTION_HEAD_TO_HEAD:
		{
		LOG( "me backwards, line forwards\n" );

		//me backwards
		vector<CSketchPoint*>::iterator iter;
		iter=End();
		iter--;
		while(iter!=Begin()) {
			(*iter)->SwapControlPoints();
			tmp->Add(*iter);
			*iter=0;
			iter--;
		}
		CSketchPoint *linkPoint = new CSketchPoint((*iter)->GetCoords(),false,false);
		linkPoint->SetControlPointBack((*iter)->GetControlPointForward());
		//line forwards
		iter=line->Begin();
		linkPoint->SetControlPointForward((*iter)->GetControlPointForward());
		tmp->Add(linkPoint);
		for(iter++; iter!=line->End(); iter++) {
			tmp->Add(*iter);
			*iter=0;
		}
		break;
		}
	case INTERSECTION_HEAD_TO_TAIL:
		{
		LOG( "me backwards, line backwards\n" );

		//me backwards
		vector<CSketchPoint*>::iterator iter;
		iter=End();
		iter--;
		while(iter!=Begin()) {
			(*iter)->SwapControlPoints();
			tmp->Add(*iter);
			*iter=0;
			iter--;
		} 

		CSketchPoint *linkPoint = new CSketchPoint((*iter)->GetCoords(),false,false);
		linkPoint->SetControlPointBack((*iter)->GetControlPointForward());
		//line backwards
		iter=line->End();
		iter--;
		linkPoint->SetControlPointForward((*iter)->GetControlPointBack());
		tmp->Add(linkPoint);
		iter--;
		while(iter!=line->Begin()) {
			(*iter)->SwapControlPoints();
			tmp->Add(*iter);
			*iter=0;
			iter--;
		}
		(*iter)->SwapControlPoints();
		tmp->Add(*iter);
		*iter=0;
		break;
		}
	case INTERSECTION_TAIL_TO_HEAD:
		{
		LOG( "me forwards, line forwards\n" );
		//me forwards
		vector<CSketchPoint*>::iterator iter;
		iter=Begin();
		vector<CSketchPoint*>::iterator end=End();
		end--;
		while (iter != end) 
		{
			//(*iter)->Trace();
			tmp->Add(*iter);
			*iter=0;
			iter++;
		}

		CSketchPoint *linkPoint = new CSketchPoint((*iter)->GetCoords(),false,false);
		linkPoint->SetControlPointBack((*iter)->GetControlPointBack());
		//line forwards
		iter=line->Begin();
		linkPoint->SetControlPointForward((*iter)->GetControlPointForward());
		tmp->Add(linkPoint);
		//LOG("linkPoint: ");
		//linkPoint->Trace();
		
		iter++;
		while(iter!=line->End()) 
		{
			//(*iter)->Trace();
			tmp->Add(*iter);
			*iter=0;
			iter++;
		}
		break;
		}
	case INTERSECTION_TAIL_TO_TAIL:
		{
		LOG( "me forwards, line backwards\n" );
		//me forwards
		vector<CSketchPoint*>::iterator iter;
		iter=Begin();
		vector<CSketchPoint*>::iterator end=End();
		end--;
		do {
			tmp->Add(*iter);
			*iter=0;
			iter++;
		} while(iter!=end);

		CSketchPoint *linkPoint = new CSketchPoint((*iter)->GetCoords(),false,false);
		linkPoint->SetControlPointBack((*iter)->GetControlPointBack());
		//line backwards
		iter=line->End();
		iter--;
		linkPoint->SetControlPointForward((*iter)->GetControlPointBack());
		tmp->Add(linkPoint);

		iter--;
		while(iter!=line->Begin()) {
			(*iter)->SwapControlPoints();
			tmp->Add(*iter);
			*iter=0;
			iter--;
		}
		(*iter)->SwapControlPoints();
		tmp->Add(*iter);
		*iter=0;
		break;
		}
	default:
		ASSERT(false);
	}

	ASSERT ( 0 == tmp->RemoveDuplicatePoints() );

	return tmp;
}

vector<CSketchPoint*>::iterator CPolyLine::Begin(void)
{
	return m_points.begin();
}

vector<CSketchPoint*>::iterator CPolyLine::End(void)
{
	return m_points.end();
}

double CPolyLine::GetMaxDeviation()
{
	double maxDeviation = -1.0;

	CFPoint p1 = ( m_points.front() )->GetCoords();
	CFPoint p2 = ( m_points.back() )->GetCoords();

	vector<CSketchPoint*>::iterator iter;
	vector<CSketchPoint*>::iterator end;

	iter=m_points.begin();
	iter++;
	end=m_points.end();
	end--;

	while(iter!=end) {
		iter++;

		CSketchPoint *p = *iter;

		double dev = (p1.GetY() - p2.GetY()) * p->GetX();
		dev += (p2.GetX()-p1.GetX())*p->GetY();
		dev += p1.GetX()*p2.GetY();
		dev -= p2.GetX()*p1.GetY();
		dev = abs(dev);
		dev /= sqrt(double((p2.GetX()-p1.GetX())*(p2.GetX()-p1.GetX()) + (p2.GetY()-p1.GetY())*(p2.GetY()-p1.GetY())));

		if(dev>maxDeviation) {
			maxDeviation = dev;
		}
	}

	//LOG("maxDeviation: %f\n",maxDeviation);
	return maxDeviation;
}

void CPolyLine::Clear(void)
{
	m_points.clear();
}

CSketchPoint* CPolyLine::GetMaxDeviationPoint(void)
{
	double maxDeviation = -1.0;
	CSketchPoint* maxDeviationPoint = NULL;

	CFPoint p1 = m_points.front()->GetCoords();
	CFPoint p2 = m_points.back()->GetCoords();

	vector<CSketchPoint*>::iterator iter;
	vector<CSketchPoint*>::iterator end;

	iter=m_points.begin();
	iter++;
	end=m_points.end();
	end--;

	while(iter!=end) {
		iter++;

		CSketchPoint *p = *iter;

		double dev = (p1.GetY()-p2.GetY())*p->GetX();
		dev += (p2.GetX()-p1.GetX())*p->GetY();
		dev += p1.GetX()*p2.GetY();
		dev -= p2.GetX()*p1.GetY();
		dev = abs(dev);
		dev /= sqrt(double((p2.GetX()-p1.GetX())*(p2.GetX()-p1.GetX()) + (p2.GetY()-p1.GetY())*(p2.GetY()-p1.GetY())));

		if(dev>maxDeviation) {
			maxDeviation = dev;
			maxDeviationPoint = p;
		}
	}
	return maxDeviationPoint;
}

CSketchPoint* CPolyLine::At(unsigned int i) {
	return m_points.at(i);
}

int CPolyLine::GetMedianThickness(void)
{
	vector<CSketchPoint*>::iterator iter;
	vector<int> thicknesses;
	for(iter=m_points.begin(); iter!=m_points.end(); iter++) {
		thicknesses.push_back(CBinarizer::Instance()->GetDistanceMap()->GetPixel(int(0.5+(*iter)->GetX()),int(0.5+(*iter)->GetY())));
	}
	sort(thicknesses.begin(),thicknesses.end());
	return thicknesses.at(thicknesses.size()/2);
}

void CPolyLine::SetTail(bool val)
{
	m_isTail = val;
}

bool CPolyLine::IsTail(void)
{
	return m_isTail;
}

CPolyLine* CPolyLine::SmoothPositions(void)
{
	CPolyLine* nuLine = new CPolyLine();
	nuLine->SetTail(IsTail());

	nuLine->Add(GetHeadPoint()->Clone());

	for(unsigned int point=1; point<Size()-1; point++) {
		double x = 0.5 * At(point)->GetX();
		double y = 0.5 * At(point)->GetY();
		x += 0.25 * At(point-1)->GetX();
		x += 0.25 * At(point+1)->GetX();
		y += 0.25 * At(point-1)->GetY();
		y += 0.25 * At(point+1)->GetY();
		//LOG("point: %i; ",point);
		//LOG("old: %f %f; ",At(point)->GetX(),At(point)->GetY());
		//LOG("new: %f %f\n",x,y);
		nuLine->Add(new CSketchPoint(CFPoint(x,y)));
	}

	nuLine->Add(GetTailPoint()->Clone());
	return nuLine;
}

void CPolyLine::Trace(void)
{
	if ( Size() == 0 ) 
	{
		LOG ( "CPolyLine is empty" );
	}

	for(unsigned int l_pointIndex=0; 
		l_pointIndex < Size(); 
		l_pointIndex++) 
	{
		CSketchPoint *p = At(l_pointIndex);
		LOG ("CPolyLine[%i]: ", l_pointIndex);
		p->Trace();
	}
}

/* if two or more points in a row are the same, just keep one 

returns number of removed points!
*/
int CPolyLine::RemoveDuplicatePoints (void)
{
	CLogger::Instance()->Inactivate();

	static const double MIN_POINT_DISTANCE = 0.1;
	int l_removedPoints = 0;

	LOG ( "RemoveDuplicatePoints\n" );

	LOG ( "size was: %i\n", Size() );

	// keep points we want to this line
	CPolyLine *l_newLine = new CPolyLine();

	for ( unsigned int l_pointIndex = 0;
		l_pointIndex < Size()-1;
		l_pointIndex++ ) 
	{
		CSketchPoint *l_point1 = At( l_pointIndex );
		CSketchPoint *l_point2 = At( l_pointIndex + 1 );

		double l_distance = l_point1->Distance( l_point2->GetCoords() );

		bool l_distanceIsZero = l_distance < MIN_POINT_DISTANCE;
		
		if ( l_distanceIsZero )
		{
			l_removedPoints++;
			LOG("delete point\n");
			//don't keep this point!
			delete l_point1;
		}
		else 
		{
			//store point in new line
			l_newLine->Add ( l_point1 );
		}
	}

	//always store last point
	CSketchPoint* l_lastPoint = At ( Size()-1 );
	l_newLine->Add ( l_lastPoint );

	//drop all local point references
	Clear();

	vector<CSketchPoint*>::iterator iter;

	for(iter = l_newLine->Begin(); 
		iter != l_newLine->End(); 
		iter++) 
	{
		Add( (*iter)->Clone() );
	}

	LOG ( "size became: %i\n", Size() );

	delete l_newLine;

	return l_removedPoints;
}

void CPolyLine::SmoothPoint(int a_pointIndex)
{
	CFPoint p = At(a_pointIndex-1)->GetCoords() * 0.5;
	p += At(a_pointIndex+1)->GetCoords() * 0.5;

	At(a_pointIndex)->SetX( p.GetX() );
	At(a_pointIndex)->SetY( p.GetY() );
}

void CPolyLine::DrawUsingGraphics(Graphics & a_graphics, Pen &a_pen)
{
	vector<CSketchPoint*>::iterator p_iter;

	CSketchPoint *p_prev = 0;

	for(p_iter=Begin(); p_iter!=End(); ++p_iter) 
	{
		CSketchPoint *p=*p_iter;

		if(p_prev) {
			//ARGB paintcolor = 0xff000000;
			//if(color&4) paintcolor |= 0xff0000;
			//if(color&2) paintcolor |= 0xff00;
			//if(color&1) paintcolor |= 0xff;
			//DrawCurve(canvas,p_prev,p,paintcolor);
			DrawCurve(a_graphics, a_pen, p_prev, p);
		}
		p_prev = p;
	}
}

void CPolyLine::DrawCurve(Graphics& a_graphics, Pen &a_pen, CSketchPoint* a_startPoint, CSketchPoint* a_endPoint)
{
	a_graphics.DrawBezier ( &a_pen,
		a_startPoint->GetPointF(),
		a_startPoint->GetControlPointForward().GetPointF(),
		a_endPoint->GetControlPointBack().GetPointF(),
		a_endPoint->GetPointF()
		);
}
