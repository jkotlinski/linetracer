#include "StdAfx.h"
#include "holefiller.h"

#include "RawImage.h"
#include "ProjectSettings.h"

CHoleFiller::CHoleFiller(void)
: CImageProcessor()
{
	LOG("init holefiller\n");
	SetName ( CString ( "HoleFiller" ) );
	SetType ( HOLEFILLER );
}

CHoleFiller::~CHoleFiller(void)
{
}

CHoleFiller *CHoleFiller::Instance() {
    static CHoleFiller inst;
    return &inst;
}

CSketchImage* CHoleFiller::Process(CSketchImage *i_src) {
	CRawImage<bool> *src=dynamic_cast<CRawImage<bool>*>(i_src);
	ASSERT ( src != NULL );

	CRawImage<bool> *dst = new CRawImage<bool>(src->GetWidth(),src->GetHeight());
	CRawImage<bool> tmp(src->GetWidth(),src->GetHeight());
	for(int i=0; i<src->GetPixels(); i++) {
		bool val = src->GetPixel(i);
		dst->SetPixel(i, val);
		tmp.SetPixel(i, val);
	}

	const unsigned int max_area = static_cast<const unsigned int>(CProjectSettings::Instance()->GetParam(
		CProjectSettings::HOLEFILLER_MAX_AREA_TO_FILL));

	for(int x=0; x<tmp.GetWidth(); x++) {
		for(int y=0; y<tmp.GetHeight(); y++) {
			if(tmp.GetPixel(x,y)) {
				deque<int> pixelsInArea;
				
				ScanArea(&tmp, &pixelsInArea, CPoint(x,y), max_area);

				if(pixelsInArea.size() < max_area) {
					while(!pixelsInArea.empty()) {
						CPoint p = pixelsInArea.front();
						dst->SetPixel(p.x,p.y, false);
						pixelsInArea.pop_front();
					}
				}
			}
		}
	}
	return dst;
}

void CHoleFiller::ScanArea(CRawImage<bool>* canvas, deque<int>* pixelsInArea, const CPoint &start, const unsigned int a_max_area)
const
{
	deque<int> pointsToCheck;

	pointsToCheck.push_back(start.x | (start.y<<16));
	pixelsInArea->push_back(start.x | (start.y<<16));

	int width = canvas->GetWidth();
	int height = canvas->GetHeight();

	while(!pointsToCheck.empty()) 
	{
		int p = pointsToCheck.front();
		int x = p&0xffff;
		int y = p>>16;
		pointsToCheck.pop_front();

		if(x<0 || y<0) continue;
		if(x==width) continue;
		if(y==height) continue;
		if(!canvas->GetPixel(x,y)) continue;

		if(pixelsInArea->size() <= a_max_area) 
		{
			pixelsInArea->push_back(x | (y<<16));
		}
		canvas->SetPixel(x,y,false);

		if(canvas->GetPixel(x+1,y)) 
		{
			pointsToCheck.push_back( (x+1) | (y<<16));
		}
		if(canvas->GetPixel(x-1,y)) 
		{
			pointsToCheck.push_back( (x-1) | (y << 16));
		}
		if(canvas->GetPixel(x,y+1)) 
		{
			pointsToCheck.push_back( x | ((y+1)<<16 ) );
		}
		if(canvas->GetPixel(x,y-1)) 
		{
			pointsToCheck.push_back( x | ((y-1)<<16 ) );
		}
	}
}
