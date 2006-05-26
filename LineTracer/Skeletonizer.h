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
	void DistanceTransform(CRawImage<bool> *src, CRawImage<ARGB> *dst, int DirectDistance, int IndirectDistance);
	int IsKnot(CRawImage<bool>* image, ARGB x, ARGB y);
	void CreateKnotImage(CRawImage<bool>* image, CRawImage<ARGB>* knotImage);
	CLineImage* Vectorize(CRawImage<bool>* segmentMap, CRawImage<ARGB>* knotMap);
	void TraceLine(CRawImage<bool>* segmentImage, CRawImage<ARGB>* knotImage, CPolyLine* line, CFPoint start);
	CFPoint IsKnotNeighbor(CRawImage<ARGB>* knotImage, CFPoint point);
	CSketchPoint FindNeighborKnot(CRawImage<ARGB>* knotImg, CPoint p);
	CPoint FindSegmentNeighbor(CRawImage<bool>* segmentImage, const CPoint &p);
	bool NoOrthogonalNeighbors(CRawImage<bool>* segmentImage, CFPoint p);
	bool IsEndPoint(CRawImage<bool>* image,CPoint p);
	double AFMMSolve(int i1, int j1, int i2, int j2, double sol, char *f, double* T, int width);
	
	static const char BAND=0;
	static const char INSIDE=1;
	static const char KNOWN=2;
	void TrackBoundary(int x, int y, char* f, double* U, double &val, int width);
	//CRawImage<bool>* MagnifyImage(CRawImage<bool>* img);
	//CLineImage* SmoothPositions(CLineImage* lineImage);

private:
	void TraceSimpleLines( CRawImage<bool> &segmentMap, CLineImage &li );
	void TraceCircles( CRawImage<bool> &segmentMap, CLineImage &li );
	void MarkKnotNeighborsWithKnotId(CRawImage< Gdiplus::ARGB >* knotImage, int x, int y, int l_knot_id);
};
