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
private:
	void RemoveSmallAreasFromImage(CRawImage<bool> & a_image, const unsigned int a_minimumAreaThreshold);
	void ScanArea(CRawImage<bool> &canvas, deque<int>* pixelsInArea, const CPoint &start, const unsigned int a_max_area) const;
};
