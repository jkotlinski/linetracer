#pragma once
#include "imageprocessor.h"

class CBinarizer :
	public CImageProcessor
{
protected:
	CBinarizer(void);
public:
	static CBinarizer* Instance();
	~CBinarizer(void);
	CSketchImage* Process(CSketchImage* src);

protected:
	int CalculateOtsuThreshold(CRawImage *src);
};
