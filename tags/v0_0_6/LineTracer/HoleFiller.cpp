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
	CProjectSettings::Instance()->SetParam( 
		CProjectSettings::HOLEFILLER_MIN_AREA, 20.0 );
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

	double min_area = CProjectSettings::Instance()->GetParam(
		CProjectSettings::HOLEFILLER_MIN_AREA);

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

void CHoleFiller::ScanArea(CRawImage<bool>* canvas, deque<CPoint,boost::fast_pool_allocator<CPoint> >* pixelsInArea, const CPoint &start)
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
	static const int SCALE = 1;
	int width = a_image->GetWidth();
	int height = a_image->GetHeight();
	CRawImage<bool> *src = dynamic_cast<CRawImage<bool>*> (a_image);
	ASSERT ( src != NULL );

	for(int x=0; x<width*SCALE; x+=SCALE) {
		for(int y=0; y<height*SCALE; y+=SCALE) {
			ARGB p=src->GetPixel(x/SCALE,y/SCALE)?0xffffff:0;
			if(!p) {
				for(int i=0; i<SCALE; i++) {
					for(int j=0; j<SCALE; j++) {
						a_canvas->SetPixel(x+i, y+j, 0xff000000 | p);
					}
				}
			}
		}
	}
}