#pragma once
#include "imageprocessor.h"

class AreasToClosedCurvesProcessor :
	public CImageProcessor
{
protected:
	AreasToClosedCurvesProcessor(void);
public:
	static AreasToClosedCurvesProcessor* Instance();
	~AreasToClosedCurvesProcessor(void);
	CSketchImage* Process(CSketchImage* src);
};
