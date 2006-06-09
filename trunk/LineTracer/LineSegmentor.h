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
	CSketchImage* Process(CProjectSettings & a_project_settings, CSketchImage* src);

private:
	void Add(CPolyLine* dst, CPolyLine* src);
};
