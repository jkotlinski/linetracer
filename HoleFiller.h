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
	CSketchImage* Process(CProjectSettings & a_project_settings, CSketchImage* src);
private:
	void ScanArea(CRawImage<bool>* canvas, deque<int>* pixelsInArea, const CPoint &start, const unsigned int a_max_area) const;
};
