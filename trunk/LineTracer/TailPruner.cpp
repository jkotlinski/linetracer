#include "StdAfx.h"
#include "tailpruner.h"

#include "ProjectSettings.h"
#include "LineImage.h"

CTailPruner::CTailPruner(void)
: CImageProcessor()
{
	SetName ( CString ( "TailPruner" ) );
	SetType ( TAILPRUNER );
	CProjectSettings::Instance()->SetParam(
		CProjectSettings::TAILPRUNER_THRESHOLD, 
		5.0);
}

CTailPruner::~CTailPruner(void)
{
}

CTailPruner* CTailPruner::Instance() {
    static CTailPruner inst;
    return &inst;
}

/*remove all lines with fewer than "threshold" points */
CSketchImage* CTailPruner::Process(CSketchImage *i_src) {
	CLineImage *src = dynamic_cast<CLineImage*>(i_src);
	ASSERT ( src != NULL );

	CLineImage *dst = new CLineImage(src->GetWidth(),src->GetHeight());

	unsigned int threshold = (unsigned int)(
		CProjectSettings::Instance()->GetParam(
		CProjectSettings::TAILPRUNER_THRESHOLD));

	for(unsigned int i=0; i<src->Size(); i++) {
		CPolyLine *line = src->GetLine(i);

		if(line->Size() >= threshold || line->HasKnots()==2) {
			dst->Add(line->Clone());
		}
	}
	dst->SolderKnots();

	return dst;
}
