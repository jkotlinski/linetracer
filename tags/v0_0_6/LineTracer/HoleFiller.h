#pragma once
#include "imageprocessor.h"

#include <deque>
#include "RawImage.h"

#define __WIN32__
#include <boost/pool/pool_alloc.hpp>

class CHoleFiller :
	public CImageProcessor
{
protected:
	CHoleFiller(void);
public:
	static CHoleFiller* Instance();
	~CHoleFiller(void);
	CSketchImage* Process(CSketchImage* src);
private:
	void ScanArea(CRawImage<bool>* canvas, deque<CPoint,boost::fast_pool_allocator<CPoint> >* pixelsInArea, const CPoint &start);
public:
	void PaintImage(CSketchImage* a_image, CRawImage<ARGB> *a_canvas) const;
};
