#pragma once
#include "imageprocessor.h"

#include <deque>

class CThinner :
	public CImageProcessor
{
public:
	~CThinner(void);
	static CThinner* Instance(void);
private:
	CThinner(void);
public:
	CSketchImage* Process(CProjectSettings & a_project_settings, CSketchImage* src);
private:
	deque<CPoint> PeelPixels(const CRawImage<bool> *img);
	int DeletePixels(deque<CPoint> & a_pixelPoints,
		CRawImage<bool> & a_canvas);
	bool IsPointThinnable(const CRawImage<bool>* img, const CPoint & p) const;
	bool IsPointThinnableZhangSuen(const CRawImage<bool>* img, const CPoint & p, int pass) const;
};
