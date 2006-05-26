#pragma once
#include "imageprocessor.h"
#include "LineImage.h"

#include <map>

using namespace std;

#define __WIN32__
#include <boost/pool/pool_alloc.hpp>

typedef map<int,short,less<int>,boost::fast_pool_allocator<pair<int,short> > > MaximumMapType;

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
	bool IsKnot(CRawImage<bool>* image, ARGB x, ARGB y);
	void CreateKnotImage(CRawImage<bool>* image, CRawImage<char>* knotImage);
	CLineImage* Vectorize(CRawImage<bool>* segmentMap, CRawImage<char>* knotMap);
	void TraceLine(CRawImage<bool>* segmentImage, CRawImage<char>* knotImage, CPolyLine* line, CFPoint start);
	CFPoint IsKnotNeighbor(CRawImage<char>* knotImage, CFPoint point);
	CSketchPoint FindNeighborKnot(CRawImage<char>* knotImg, CPoint p);
	CPoint FindSegmentNeighbor(CRawImage<bool>* segmentImage, const CPoint &p);
	bool NoOrthogonalNeighbors(CRawImage<bool>* segmentImage, CFPoint p);
	bool IsEndPoint(CRawImage<bool>* image,CPoint p);
	
private:
	void TraceSimpleLines( CRawImage<bool> &segmentMap, CLineImage &li );
	void TraceCircles( CRawImage<bool> &segmentMap, CLineImage &li );
	void MarkKnotNeighbors(CRawImage<char>* knotImage, int x, int y);
};
