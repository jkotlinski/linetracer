#include "StdAfx.h"
#include "holefiller.h"

#include "RawImage.h"

CHoleFiller::CHoleFiller(void)
{
	LOG("init binarizer\n");
	SetParam(HOLEFILLER_MIN_AREA,20);
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

	double min_area = GetParam(HOLEFILLER_MIN_AREA);

	for(int x=0; x<tmp.GetWidth(); x++) {
		for(int y=0; y<tmp.GetHeight(); y++) {
			if(tmp.GetPixel(x,y)) {
				deque<CPoint,boost::fast_pool_allocator<CPoint> > pixelsInArea;
				
				ScanArea(&tmp, &pixelsInArea, CPoint(x,y));

				if(pixelsInArea.size() < min_area) {
					while(!pixelsInArea.empty()) {
						CPoint p = pixelsInArea.front();
						dst->SetPixel(p.x,p.y,0);
						pixelsInArea.pop_front();
					}
				}
			}
		}
	}
	return dst;
}

void CHoleFiller::ScanArea(CRawImage<bool>* canvas, deque<CPoint,boost::fast_pool_allocator<CPoint> >* pixelsInArea, CPoint start)
{
	deque<CPoint,boost::fast_pool_allocator<CPoint> > pointsToCheck;

	pointsToCheck.push_back(start);
	pixelsInArea->push_back(start);

	int width = canvas->GetWidth();
	int height = canvas->GetHeight();

	while(!pointsToCheck.empty()) {
		CPoint p = pointsToCheck.front();
		pointsToCheck.pop_front();

		if(p.x<0 || p.y<0) continue;
		if(p.x==width) continue;
		if(p.y==height) continue;
		if(!canvas->GetPixel(p.x,p.y)) continue;

		pixelsInArea->push_back(p);
		canvas->SetPixel(p.x,p.y,0);
		pointsToCheck.push_back(CPoint(p.x+1,p.y));
		pointsToCheck.push_back(CPoint(p.x-1,p.y));
		pointsToCheck.push_back(CPoint(p.x,p.y+1));
		pointsToCheck.push_back(CPoint(p.x,p.y-1));
	}
}

void CHoleFiller::PaintImage(CSketchImage* a_image, CRawImage<ARGB> *a_canvas) const
{
}
