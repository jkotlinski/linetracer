#include "StdAfx.h"
#include "linesegmentor.h"

#include "LineImage.h"
#include <list>
#include "ProjectSettings.h"

using namespace std;

CLineSegmentor::CLineSegmentor(void)
: CImageProcessor()
{
	SetName ( CString ( "LineSegmentor" ) );
	LOG("init linesegmentor\n");
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

	CLineImage *src = dynamic_cast<CLineImage*>(i_src);

	ASSERT ( src != NULL );
	CLineImage *dst = new CLineImage(src->GetWidth(),src->GetHeight());

	if( ( useMaxErrors ) ) {
		//SMART SELECTION: CHOOSE MAXIMUM ERRORS
		for(unsigned int i=0; i<src->Size(); i++) {
			CPolyLine *line = new CPolyLine();
			line->Add(src->GetLine(i)->GetHeadPoint()->Clone());
			Add(line,src->GetLine(i));
			line->Add(src->GetLine(i)->GetTailPoint()->Clone());

			dst->Add(line);
		}
	} else {
		//USE EVERY FOURTH POINT
		for(unsigned int i=0; i<src->Size(); i++) {
			ASSERT ( src->GetLine(i)->Size() > 1 );

			CPolyLine *newLine = new CPolyLine();

			newLine->SetTail(src->GetLine(i)->IsTail());
			newLine->Add(src->GetLine(i)->GetHeadPoint()->Clone());
			bool added=false;

			for(int point = 4; point < int(src->GetLine(i)->Size())-(added?4:1); point+=4) {
				newLine->Add(src->GetLine(i)->At(point)->Clone());
				added=true;
			}
			newLine->Add(src->GetLine(i)->GetTailPoint()->Clone());

			ASSERT ( newLine->Size() > 1 );
			dst->Add(newLine);
		}	
	}
	//clean up any mess i did...
	dst->DiscardDuplicateLines();
	dst->AssertNoEmptyLines();
	return dst;
}

void CLineSegmentor::Add(CPolyLine*dst, CPolyLine* src)
{
	CSketchPoint *p;

	double THRESHOLD = CProjectSettings::Instance()->GetParam(
		CProjectSettings::LINESEGMENTOR_THRESHOLD);

	if(src->GetMaxDeviation()>=THRESHOLD) {
		p = src->GetMaxDeviationPoint();

		CPolyLine l1;
		CPolyLine l2;

		vector<CSketchPoint*>::iterator iter;

		iter=src->Begin();
		while(*iter!=p) {
			l1.Add(*iter);
			++iter;
		}
		l1.Add(*iter);
		while(iter!=src->End()) {
			l2.Add(*iter);
			++iter;
		}

		Add(dst,&l1);
		dst->Add(p->Clone());
		Add(dst,&l2);

		l1.Clear();
		l2.Clear();
	}
}
