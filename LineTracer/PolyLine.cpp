#include "StdAfx.h"
#include ".\polyline.h"

#include <assert.h>
#include <list>
#include <math.h>
#include <algorithm>

#include "Binarizer.h"

CPolyLine::CPolyLine(void)
: m_isTail(false)
{
}

CPolyLine::~CPolyLine(void)
{
	vector<CSketchPoint*>::iterator iter;
	for(iter=Begin(); iter!=End(); iter++) {
		if(*iter!=NULL) {
			delete *iter;
			*iter = NULL;
		}
	}
}

void CPolyLine::Add(CSketchPoint *point)
{
	//TRACE("added point %i:%i\n",point.x,point.y);
	m_points.push_back(point);
}


bool CPolyLine::Contains(CPoint p)
{
	vector<CSketchPoint*>::iterator iter;
	for(iter=Begin(); iter!=End(); iter++) {
		if((*iter)->x==p.x && (*iter)->y==p.y) {
			return true;
		}
	}
	return false;
}

int CPolyLine::Size(void)
{
	return (int)m_points.size();
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

CPolyLine* CPolyLine::MergeLine(CPolyLine* line) {
	static const double MAX_DIST = 2;

	CPolyLine *tmp=new CPolyLine();

	vector<CSketchPoint*>::iterator iter;

	if(GetHeadPoint()->Distance(*line->GetHeadPoint()) < MAX_DIST) {
		//me backwards
		iter=End();
		iter--;
		while(iter!=Begin()) {
			(*iter)->SwapControlPoints();
			tmp->Add(*iter);
			*iter=0;
			iter--;
		}
		CSketchPoint *linkPoint = new CSketchPoint(*(*iter),false,false);
		linkPoint->SetControlPointBack((*iter)->GetControlPointForward());
		//line forwards
		iter=line->Begin();
		linkPoint->SetControlPointForward((*iter)->GetControlPointForward());
		tmp->Add(linkPoint);
		for(iter++; iter!=line->End(); iter++) {
			tmp->Add(*iter);
			*iter=0;
		}
	} else if(GetHeadPoint()->Distance(*line->GetTailPoint()) < MAX_DIST) {
		//me backwards
		iter=End();
		iter--;
		while(iter!=Begin()) {
			(*iter)->SwapControlPoints();
			tmp->Add(*iter);
			*iter=0;
			iter--;
		} 

		CSketchPoint *linkPoint = new CSketchPoint(*(*iter),false,false);
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
	} else if(GetTailPoint()->Distance(*line->GetHeadPoint()) < MAX_DIST) {
		//me forwards
		iter=Begin();
		vector<CSketchPoint*>::iterator end=End();
		end--;
		do {
			tmp->Add(*iter);
			*iter=0;
			iter++;
		} while(iter!=end);

		CSketchPoint *linkPoint = new CSketchPoint(*(*iter),false,false);
		linkPoint->SetControlPointBack((*iter)->GetControlPointBack());
		//line forwards
		iter=line->Begin();
		linkPoint->SetControlPointForward((*iter)->GetControlPointForward());
		tmp->Add(linkPoint);

		iter++;
		while(iter!=line->End()) {
			tmp->Add(*iter);
			*iter=0;
			iter++;
		}
	} else if(GetTailPoint()->Distance(*line->GetTailPoint()) < MAX_DIST) {
		//me forwards
		iter=Begin();
		vector<CSketchPoint*>::iterator end=End();
		end--;
		do {
			tmp->Add(*iter);
			*iter=0;
			iter++;
		} while(iter!=end);

		CSketchPoint *linkPoint = new CSketchPoint(*(*iter),false,false);
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
	} else {
		assert(0);
	}

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
	CSketchPoint* maxDeviationPoint;

	CFPoint *p1=m_points.front();
	CFPoint *p2=m_points.back();

	vector<CSketchPoint*>::iterator iter;
	vector<CSketchPoint*>::iterator end;

	iter=m_points.begin();
	iter++;
	end=m_points.end();
	end--;

	while(iter!=end) {
		iter++;

		CSketchPoint *p = *iter;

		double dev = (p1->y-p2->y)*p->x;
		dev += (p2->x-p1->x)*p->y;
		dev += p1->x*p2->y;
		dev -= p2->x*p1->y;
		dev = abs(dev);
		dev /= sqrt(double((p2->x-p1->x)*(p2->x-p1->x) + (p2->y-p1->y)*(p2->y-p1->y)));

		if(dev>maxDeviation) {
			maxDeviation = dev;
			maxDeviationPoint = p;
		}
	}

	//TRACE("maxDeviation: %f\n",maxDeviation);
	return maxDeviation;
}

void CPolyLine::Clear(void)
{
	m_points.clear();
}

CSketchPoint* CPolyLine::GetMaxDeviationPoint(void)
{
	double maxDeviation = -1.0;
	CSketchPoint* maxDeviationPoint;

	CFPoint *p1=m_points.front();
	CFPoint *p2=m_points.back();

	vector<CSketchPoint*>::iterator iter;
	vector<CSketchPoint*>::iterator end;

	iter=m_points.begin();
	iter++;
	end=m_points.end();
	end--;

	while(iter!=end) {
		iter++;

		CSketchPoint *p = *iter;

		double dev = (p1->y-p2->y)*p->x;
		dev += (p2->x-p1->x)*p->y;
		dev += p1->x*p2->y;
		dev -= p2->x*p1->y;
		dev = abs(dev);
		dev /= sqrt(double((p2->x-p1->x)*(p2->x-p1->x) + (p2->y-p1->y)*(p2->y-p1->y)));

		if(dev>maxDeviation) {
			maxDeviation = dev;
			maxDeviationPoint = p;
		}
	}
	return maxDeviationPoint;
}

CSketchPoint* CPolyLine::At(int i)
{
	return m_points.at(i);
}

int CPolyLine::GetMedianThickness(void)
{
	vector<CSketchPoint*>::iterator iter;
	vector<int> thicknesses;
	for(iter=m_points.begin(); iter!=m_points.end(); iter++) {
		thicknesses.push_back(CBinarizer::Instance()->m_distanceMap->GetPixel(int(0.5+(*iter)->x),int(0.5+(*iter)->y)));
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

	for(int point=1; point<Size()-1; point++) {
		double x = 0.5 * At(point)->x;
		double y = 0.5 * At(point)->y;
		x += 0.25 * At(point-1)->x;
		x += 0.25 * At(point+1)->x;
		y += 0.25 * At(point-1)->y;
		y += 0.25 * At(point+1)->y;
		//TRACE("point: %i; ",point);
		//TRACE("old: %f %f; ",At(point)->x,At(point)->y);
		//TRACE("new: %f %f\n",x,y);
		nuLine->Add(new CSketchPoint(CFPoint(x,y)));
	}

	nuLine->Add(GetTailPoint()->Clone());
	return nuLine;
}
