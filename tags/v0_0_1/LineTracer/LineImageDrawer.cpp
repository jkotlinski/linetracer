#include "StdAfx.h"
#include ".\lineimagedrawer.h"

#include "PolyLine.h"

CLineImageDrawer::CLineImageDrawer(void)
{
}

CLineImageDrawer::~CLineImageDrawer(void)
{
}

void CLineImageDrawer::Draw(CRawImage* canvas, CLineImage* li)
{
	for(int i=0; i<li->Size(); i++) {
		CPolyLine *line=li->At(i);

		bool isTail=line->IsTail();

		for(int j=0; j<line->Size(); j++) {
			CSketchPoint p=line->At(j);

			if(p.IsKnot()) {
				canvas->SetPixel(p.x,p.y,0xff0000);
			} else if(p.IsEndPoint()) {
				canvas->SetPixel(p.x,p.y,0x00ff00);
			} else {
				canvas->SetPixel(p.x,p.y,isTail?0xffff00:0xffffff);
			}
		}
	}
}
