#include "StdAfx.h"
#include ".\linesegmentor.h"

#include "LineImage.h"

#include <assert.h>
#include <list>

using namespace std;

CLineSegmentor::CLineSegmentor(void)
{
	SetParam("threshold",1.5);
		TRACE("init linesegmentor\n");
}

CLineSegmentor::~CLineSegmentor(void)
{
}

CLineSegmentor* CLineSegmentor::Instance() {
	static CLineSegmentor inst;
	return &inst;
}

CSketchImage * CLineSegmentor::Process(CSketchImage* i_src) {
	static const bool useMaxErrors = false;

	CLineImage *src = static_cast<CLineImage*>(i_src);
	CLineImage *dst = new CLineImage(src->GetWidth(),src->GetHeight());

	if(useMaxErrors) {
		//SMART SELECTION: CHOOSE MAXIMUM ERRORS
		for(unsigned int i=0; i<src->Size(); i++) {
			CPolyLine *line = new CPolyLine();
			line->Add(src->At(i)->GetHeadPoint()->Clone());
			Add(line,src->At(i));
			line->Add(src->At(i)->GetTailPoint()->Clone());

			dst->Add(line);
		}
	} else {
		//USE EVERY FOURTH POINT
		for(unsigned int i=0; i<src->Size(); i++) {
			CPolyLine *newLine = new CPolyLine();

			newLine->SetTail(src->At(i)->IsTail());
			newLine->Add(src->At(i)->GetHeadPoint()->Clone());
			bool added=false;

			for(int point = 4; point < int(src->At(i)->Size())-(added?4:1); point+=4) {
				newLine->Add(src->At(i)->At(point)->Clone());
				added=true;
			}
			newLine->Add(src->At(i)->GetTailPoint()->Clone());

			dst->Add(newLine);
		}	
	}
	return dst;
}

void CLineSegmentor::Add(CPolyLine*dst, CPolyLine* src)
{
	CSketchPoint *p;

	double THRESHOLD = GetParam("threshold");

	if(src->GetMaxDeviation()>=THRESHOLD) {
		p = src->GetMaxDeviationPoint();

		CPolyLine l1;
		CPolyLine l2;

		vector<CSketchPoint*>::iterator iter;

		iter=src->Begin();
		while(*iter!=p) {
			l1.Add(*iter);
			iter++;
		}
		l1.Add(*iter);
		while(iter!=src->End()) {
			l2.Add(*iter);
			iter++;
		}

		Add(dst,&l1);
		dst->Add(p->Clone());
		Add(dst,&l2);

		l1.Clear();
		l2.Clear();
	}
}
