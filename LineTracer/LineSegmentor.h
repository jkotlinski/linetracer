#pragma once
#include "imageprocessor.h"

#include "PolyLine.h"
#include "LineImage.h"

class CLineSegmentor :
	public CImageProcessor
{
public:
	~CLineSegmentor(void);
	static CLineSegmentor* Instance(void);
protected:
	CLineSegmentor(void);
public:
	CSketchImage* Process(CSketchImage* src);

private:
	void Add(CPolyLine* dst, CPolyLine* src);
};
