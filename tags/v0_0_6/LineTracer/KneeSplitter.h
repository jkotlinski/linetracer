#pragma once

#include "ImageProcessor.h"

class CKneeSplitter
	: public CImageProcessor
{
protected:
	CKneeSplitter(void);
public:
	static CKneeSplitter* Instance();
	~CKneeSplitter(void);
	CSketchImage* Process(CSketchImage* src);
	void PaintImage(CSketchImage* a_image, CRawImage<ARGB> *a_canvas) const;
};
