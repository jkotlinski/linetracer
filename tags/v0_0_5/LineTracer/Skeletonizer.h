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
	//CRawImage<ARGB>* DeleteNonMaximums(CRawImage<ARGB>* dst);
	CRawImage<ARGB>* DeleteNonMaximumsSimple(CRawImage<ARGB>* dst);
	//MaximumMapType* DoAFMM(CRawImage<bool>* dst,bool direction);
	int IsKnot(CRawImage<bool>* image, ARGB x, ARGB y);
	CRawImage<ARGB>* CreateSegmentMap(CRawImage<bool>* image, CRawImage<ARGB>* knotImage);
	CLineImage* Vectorize(CRawImage<ARGB>* segmentMap, CRawImage<ARGB>* knotMap);
	void TraceLine(CRawImage<ARGB>* segmentImage, CRawImage<ARGB>* knotImage, CPolyLine* line, CFPoint start, map<int,bool> *forbiddenEndKnotIds);
	CFPoint IsKnotNeighbor(CRawImage<ARGB>* knotImage, CFPoint point, map<int,bool> *forbiddenEndKnotIds);
	CSketchPoint FindNeighborKnot(CRawImage<ARGB>* knotImg, CPoint p);
	CFPoint FindSegmentNeighbor(CRawImage<ARGB>* segmentImage, CFPoint p);
	bool NoOrthogonalNeighbors(CRawImage<ARGB>* segmentImage, CFPoint p);
	bool IsEndPoint(CRawImage<ARGB>* image,CPoint p);
	double AFMMSolve(int i1, int j1, int i2, int j2, double sol, char *f, double* T, int width);
	
	static const char BAND=0;
	static const char INSIDE=1;
	static const char KNOWN=2;
	void TrackBoundary(int x, int y, char* f, double* U, double &val, int width);
	CRawImage<bool>* MagnifyImage(CRawImage<bool>* img);
	//CLineImage* SmoothPositions(CLineImage* lineImage);
public:
	void PaintImage(CSketchImage* a_image, CRawImage<ARGB> *a_canvas) const;
};
