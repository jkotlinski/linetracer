#include "StdAfx.h"
#include "kneesplitter.h"

#include <math.h>

#include "LineImage.h"
#include "SketchPoint.h"

CKneeSplitter::CKneeSplitter(void)
{
	SetParam(KNEESPLITTER_THRESHOLD,-0.3);
}

CKneeSplitter::~CKneeSplitter(void)
{
}

CKneeSplitter* CKneeSplitter::Instance() {
	static CKneeSplitter inst;
	return &inst;
}

CSketchImage * CKneeSplitter::Process(CSketchImage* i_src) {
	CLineImage *src = dynamic_cast<CLineImage*>(i_src);
	ASSERT ( src != NULL );

	// LOG ( "KneeSplitter()\n" );

	double threshold = GetParam(KNEESPLITTER_THRESHOLD);

	for(unsigned int i=0; i<src->Size(); i++) 
	{
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

			double diffx1 = prev_point->GetX() - point->GetX();
			double diffy1 = prev_point->GetY() - point->GetY();
			double diffx2 = next_point->GetX() - point->GetX();
			double diffy2 = next_point->GetY() - point->GetY();
			double dist1 = sqrt(diffx1*diffx1+diffy1*diffy1);
			double dist2 = sqrt(diffx2*diffx2+diffy2*diffy2);

			diffx1 /= dist1;
			diffy1 /= dist1;
			diffx2 /= dist2;
			diffy2 /= dist2;

			double diff = (diffx1*diffx2+diffy1*diffy2)/2;

			if(diff>threshold) {
				point->SetKnee(true);
				//LOG("SETKNEE\n");
			} else {
				point->SetKnee(false);
			}

			//advance to next point
			prev_point = point;
			point = next_point;
		}
	}

	CLineImage *dst = new CLineImage(src->GetWidth(), src->GetHeight());

	for(unsigned int l_lineIndex=0; l_lineIndex<src->Size(); l_lineIndex++) 
	{
		CPolyLine* line = src->At(l_lineIndex);
		CPolyLine* addLine = new CPolyLine();

		vector<CSketchPoint*>::iterator iter = line->Begin();

		while(iter!=line->End()) {
			addLine->Add((*iter)->Clone());
			if((*iter)->IsKnee()) {
				//break line
				//LOG("ISKNEE\n");
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
void CKneeSplitter::PaintImage(CSketchImage* a_image, CRawImage<ARGB> *a_canvas) const
{
}
