#pragma once

#include <vector>

#include "SketchPoint.h"

using namespace std;

class CPolyLine
{
public:
	CPolyLine(void);
	~CPolyLine(void);
	void Add(CSketchPoint *point);
private:
	vector<CSketchPoint*> m_points;
public:
	bool Contains(CPoint p);
	unsigned int Size(void);
	CPolyLine* Clone(void);
	int HasKnots(void);
	CSketchPoint* GetHeadPoint(void);
	CSketchPoint* GetTailPoint(void);
	CPolyLine* MergeLine(CPolyLine* line);
	vector<CSketchPoint*>::iterator Begin(void);
	vector<CSketchPoint*>::iterator End(void);
	double GetMaxDeviation();
	void Clear(void);
	CSketchPoint* GetMaxDeviationPoint(void);
	CSketchPoint* At(unsigned int i);
	int GetMedianThickness(void);
private:
	bool m_isTail;
public:
	void SetTail(bool val);
	bool IsTail(void);
	CPolyLine* SmoothPositions(void);
	void Trace(void);
	int RemoveDuplicatePoints(void);
	void SmoothPoint(int a_pointIndex);
};
