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
	int CalculateOtsuThreshold(CRawImage<unsigned char> *src);
private:
	int* m_sketchBoard;
public:
	void Init(void);
	CRawImage<int>* m_distanceMap;
private:
	void CalcDistanceMap(CRawImage<bool>* img);
};
