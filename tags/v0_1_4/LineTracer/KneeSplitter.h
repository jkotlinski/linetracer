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
};
