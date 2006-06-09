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
	CSketchImage* Process(CProjectSettings & a_project_settings, CSketchImage* src);

private:
	CRawImage<bool> * m_knot_image;

	void TraceKnotLines(CRawImage< bool >* src, CLineImage & li);
	void TraceSimpleLines( CRawImage<bool> &segmentMap, CLineImage &li );
	void TraceCircles( CRawImage<bool> &segmentMap, CLineImage &li );

	bool IsKnot(CRawImage<bool>* image, ARGB x, ARGB y);
	void CreateKnotImage(CRawImage<bool>* image);
	void Vectorize(CRawImage<bool>* segmentMap, CLineImage & li);
	void TraceLine(CRawImage<bool>* segmentImage, CPolyLine* line, CFPoint start);
	CFPoint IsKnotNeighbor(CFPoint point);
	CSketchPoint FindNeighborKnot(CPoint p);
	CPoint FindSegmentNeighbor(CRawImage<bool>* segmentImage, const CPoint &p);
	bool NoOrthogonalNeighbors(CRawImage<bool>* segmentImage, CFPoint p);
	bool IsEndPoint(CRawImage<bool>* image,CPoint p);

	int GetSeparateNeighborsCount(CRawImage< bool >* image, Gdiplus::ARGB x, Gdiplus::ARGB y);
};
