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
	int CalculateOtsuThreshold(const CRawImage<unsigned char> *img) const;
private:
	int* m_sketchBoard;
public:
	void Init(void);
public:
	const CRawImage<int>* GetDistanceMap() const;
private:
	CRawImage<int>* m_distanceMap;
	void CalcDistanceMap(const CRawImage<bool>* img);
public:
	void PaintImage(CSketchImage* a_image, CRawImage<ARGB> *a_canvas) const;
};
