#pragma once
#include "imageprocessor.h"

class CDeSaturator :
	public CImageProcessor
{
public:
	~CDeSaturator(void);
	static CDeSaturator* Instance(void);
protected:
	CDeSaturator(void);
public:
	CSketchImage* Process(CSketchImage* src);
	void PaintImage(CSketchImage* a_image, CRawImage<ARGB> *a_canvas) const;
};
