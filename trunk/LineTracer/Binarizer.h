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
	CRawImage* Process(CRawImage* src);

private:
	static CBinarizer* _instance;
protected:
	int CalculateOtsuThreshold(CRawImage *src);
};
