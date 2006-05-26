#pragma once
#include "imageprocessor.h"
#include "LineImage.h"

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
	CRawImage<char> * m_knot_image;

	bool IsKnot(CRawImage<bool>* image, ARGB x, ARGB y);
	void CreateKnotImage(CRawImage<bool>* image);
	CLineImage* Vectorize(CRawImage<bool>* segmentMap);
	void TraceLine(CRawImage<bool>* segmentImage, CPolyLine* line, CFPoint start);
	CFPoint IsKnotNeighbor(CFPoint point);
	CSketchPoint FindNeighborKnot(CPoint p);
	CPoint FindSegmentNeighbor(CRawImage<bool>* segmentImage, const CPoint &p);
	bool NoOrthogonalNeighbors(CRawImage<bool>* segmentImage, CFPoint p);
	bool IsEndPoint(CRawImage<bool>* image,CPoint p);
	void TraceSimpleLines( CRawImage<bool> &segmentMap, CLineImage &li );
	void TraceCircles( CRawImage<bool> &segmentMap, CLineImage &li );
	void MarkKnot(int x, int y);
};
