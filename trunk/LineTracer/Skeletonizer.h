#pragma once
#include "imageprocessor.h"
#include "LineImage.h"

#include <map>

class CSkeletonizer :
	public CImageProcessor
{
public:
	~CSkeletonizer(void);
	static CSkeletonizer* Instance(void);
protected:
	CSkeletonizer(void);
public:
	CSketchImage* Process(CSketchImage* src);
private:
	void DistanceTransform(CRawImage *src, CRawImage *dst, int DirectDistance, int IndirectDistance);
	CRawImage* DeleteNonMaximums(CRawImage* dst);
	int IsKnot(CRawImage* image, ARGB x, ARGB y);
	CRawImage* CreateSegmentMap(CRawImage* image,CRawImage* knotImage);
	CLineImage* Vectorize(CRawImage* segmentMap, CRawImage* knotMap);
	void TraceLine(CRawImage* segmentImage, CRawImage* knotImage, CPolyLine* line, CPoint start, map<int,bool> *forbiddenEndKnotIds);
	CPoint IsKnotNeighbor(CRawImage* knotImage, CPoint point, map<int,bool> *forbiddenEndKnotIds);
	CSketchPoint FindNeighborKnot(CRawImage* knotImg, CPoint p);
	CPoint FindSegmentNeighbor(CRawImage* segmentImage, CPoint p);
protected:
	bool NoOrthogonalNeighbors(CRawImage* segmentImage, CPoint p);
};
