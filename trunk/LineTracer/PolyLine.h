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
	bool Contains(const CPoint &p);
	unsigned int Size(void) const;
	CPolyLine* Clone(void);
	int HasKnots(void) const;
	CSketchPoint* GetHeadPoint(void) const;
	CSketchPoint* GetTailPoint(void) const;
	CPolyLine* MergeLine(CPolyLine* line);
	vector<CSketchPoint*>::iterator Begin(void);
	vector<CSketchPoint*>::iterator End(void);
	double GetMaxDeviation();
	void Clear(void);
	CSketchPoint* GetMaxDeviationPoint(void);
	CSketchPoint* At(unsigned int i) const;
	int GetMedianThickness(void);
private:
	bool m_isTail;
public:
	void SetTail(bool val);
	bool IsTail(void) const;
	CPolyLine* SmoothPositions(void);
	void Trace(void) const;
	int RemoveDuplicatePoints(void);
	void SmoothPoint(int a_pointIndex);
	
	
	// draw stuff
public:
	void DrawUsingGraphics(Graphics & a_graphics, Pen &a_pen);
private:
	void DrawCurve(Graphics& a_graphics, Pen &a_pen, CSketchPoint* a_startPoint, CSketchPoint* a_endPoint);

public:
	void AssertNotEqualTo(const CPolyLine & a_otherLine);
	const bool Equals(const CPolyLine& a_line) const;
};
