#include "StdAfx.h"
#include ".\tailpruner.h"

#include "LineImage.h"

CTailPruner::CTailPruner(void)
{
	SetParam("threshold",5);
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
	CLineImage *src = static_cast<CLineImage*>(i_src);

	CLineImage *dst = new CLineImage(src->GetWidth(),src->GetHeight());

	int threshold = int(GetParam("threshold"));

	for(int i=0; i<src->Size(); i++) {
		CPolyLine *line = src->At(i);

		if(line->Size() >= threshold || line->HasKnots()==2) {
			dst->Add(line->Clone());
		}
	}
	dst->SolderKnots();

	return dst;
}
