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
	CSketchImage* Process(CSketchImage* src);
private:
	int Thin(CRawImage<bool> *img);
	bool IsPointThinnable(CRawImage<bool>* img, CPoint p);
	bool IsPointThinnableZhangSuen(CRawImage<bool>* img, CPoint p, int pass);
};
