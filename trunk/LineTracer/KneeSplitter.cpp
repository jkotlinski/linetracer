#include "StdAfx.h"
#include ".\kneesplitter.h"

#include <math.h>

#include "LineImage.h"
#include "SketchPoint.h"

CKneeSplitter::CKneeSplitter(void)
{
	SetParam("threshold",-0.3);
}

CKneeSplitter::~CKneeSplitter(void)
{
}

CKneeSplitter* CKneeSplitter::Instance() {
	static CKneeSplitter inst;
	return &inst;
}

CSketchImage * CKneeSplitter::Process(CSketchImage* i_src) {
	CLineImage *src = static_cast<CLineImage*>(i_src);

	double threshold = GetParam("threshold");

	for(int i=0; i<src->Size(); i++) {
		CPolyLine *line = src->At(i);

		vector<CSketchPoint*>::iterator iter;
		vector<CSketchPoint*>::iterator end;
		iter = line->Begin();
		end = line->End();
		end--;

		CSketchPoint *prev_point = *iter;
		iter++;
		CSketchPoint *point = *iter;

		while(iter!=end) {
			iter++;
			CSketchPoint *next_point = *iter;

			double diffx1 = prev_point->x - point->x;
			double diffy1 = prev_point->y - point->y;
			double diffx2 = next_point->x - point->x;
			double diffy2 = next_point->y - point->y;
			double dist1 = sqrt(diffx1*diffx1+diffy1*diffy1);
			double dist2 = sqrt(diffx2*diffx2+diffy2*diffy2);

			diffx1 /= dist1;
			diffy1 /= dist1;
			diffx2 /= dist2;
			diffy2 /= dist2;

			double diff = (diffx1*diffx2+diffy1*diffy2)/2;

			if(diff>threshold) {
				point->SetKnee(true);
				//TRACE("SETKNEE\n");
			} else {
				point->SetKnee(false);
			}

			//advance to next point
			prev_point = point;
			point = next_point;
		}
	}

	CLineImage *dst = new CLineImage(src->GetWidth(), src->GetHeight());

	for(int i=0; i<src->Size(); i++) {
		CPolyLine* line = src->At(i);
		CPolyLine* addLine = new CPolyLine();

		vector<CSketchPoint*>::iterator iter = line->Begin();

		while(iter!=line->End()) {
			addLine->Add((*iter)->Clone());
			if((*iter)->IsKnee()) {
				//break line
				//TRACE("ISKNEE\n");
				dst->Add(addLine);
				addLine = new CPolyLine();
				addLine->Add((*iter)->Clone());
			}
			iter++;
		}
		dst->Add(addLine);
	}

	return dst;
}