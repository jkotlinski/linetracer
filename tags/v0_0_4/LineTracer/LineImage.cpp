#include "StdAfx.h"
#include ".\lineimage.h"

#include "PolyLine.h"

#include <vector>
#include <map>
#include <math.h>

#include <assert.h>

CLineImage::CLineImage(int width, int height)
{
	//TRACE("init lineimage\n");
	SetSize(width,height);
}

CLineImage::~CLineImage(void)
{
	//TRACE("delete lineimage with size %i\n",m_polyLines.size());
	for(unsigned int i=0; i<m_polyLines.size(); i++) {
		delete m_polyLines.at(i);
	}
}

void CLineImage::Add(CPolyLine* polyLine)
{
	m_polyLines.push_back(polyLine);
}

int CLineImage::Size(void)
{
	return (int)m_polyLines.size();
}

CPolyLine* CLineImage::At(int i)
{
	return m_polyLines.at(i);
}


void CLineImage::Clear(void)
{
	for(unsigned int i=0; i<m_polyLines.size(); i++) {
		delete m_polyLines.at(i);
	}
	m_polyLines.clear();
}

void CLineImage::SolderKnots(void)
{
	CLineImage *tmp = new CLineImage(-1,-1);

	map<int,int> knotCount;
	map<int,int>::iterator iter;
	map<int,bool> lineProcessed;

	for(int line=0; line<Size(); line++) {
		CSketchPoint *p = At(line)->GetHeadPoint();
		int pp = (int(p->x)&0xffff) | (int(p->y)<<16);
		knotCount[pp]=knotCount[pp]+1;

		p = At(line)->GetTailPoint();
		pp = (int(p->x)&0xffff) | (int(p->y)<<16);
		knotCount[pp]=knotCount[pp]+1;

		assert(*At(line)->GetHeadPoint() != *At(line)->GetTailPoint());
	}

	for(iter=knotCount.begin(); iter!=knotCount.end(); iter++) {
		if((*iter).second==2) {
			CFPoint p((*iter).first &0xffff,(*iter).first>>16);

			CPolyLine *l1=0;
			CPolyLine *l2=0;

			for(int i=0; i<Size(); i++) {
				const static double maxDist = 2;
				if(lineProcessed[i]) continue;

				CPolyLine *line = At(i);
				if(line->GetHeadPoint()->Distance(p)<maxDist || line->GetTailPoint()->Distance(p)<maxDist) {
					if(l1==0) {
						l1=line;
					} else {
						l2=line;
					}
					lineProcessed[i]=true;
				}
			}

			if(l2==0) {
				//circle!
				if(l1!=0) {
					Add(l1->Clone());
				}
			} else {
				Add(l1->MergeLine(l2));
			}
		}
	}

	for(int i=0; i<Size(); i++) {
		if(!lineProcessed[i]) {
			tmp->Add(At(i)->Clone());
		}
	}

	Clear();

	for(int i=0; i<tmp->Size(); i++) {
		Add(tmp->At(i)->Clone());
	}

	delete tmp;
}

int CLineImage::IsKnotInLines(CFPoint p)
{
	int counter=0;

	for(int i=0; i<Size(); i++) {
		CPolyLine *line = At(i);

		CSketchPoint *linePoint = line->GetHeadPoint();

		if(linePoint->Distance(p)<0.8) {
			counter++;
		} else {
			linePoint = line->GetTailPoint();
			if(linePoint->Distance(p)<0.8) {
				counter++;
			}
		}
	}

	return counter;
}



CLineImage* CLineImage::Clone(void)
{
	CLineImage *clone = new CLineImage(GetWidth(),GetHeight());

	for(int i=0; i<Size(); i++) {
		clone->Add(At(i)->Clone());
	}

	return clone;
}

bool CLineImage::IsTail(CPolyLine* pl)
{
	//TRACE("IsTail: ");
	if(IsKnotInLines(*(pl->GetHeadPoint()))==1) {
		//TRACE("true\n");
		return true;
	}
	if(IsKnotInLines(*(pl->GetTailPoint()))==1) {
		//TRACE("true\n");
		return true;
	}
	//TRACE("false\n");
	return false;
}

CLineImage* CLineImage::SmoothPositions()
{
	CLineImage *tmp = new CLineImage(GetWidth(),GetHeight());

	for(int line=0; line<Size(); line++) {
		CPolyLine* pl = At(line);
		CPolyLine* nuLine = pl->SmoothPositions();
		tmp->Add(nuLine);
	}
	return tmp;
}

// check all lines in image and update their tail status
void CLineImage::UpdateTailData(void)
{
	for(unsigned int i=0; i<m_polyLines.size(); i++) {
		CPolyLine *pl = m_polyLines.at(i);
		pl->SetTail(IsTail(pl));
	}
}
