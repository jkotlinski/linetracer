#pragma once

#include "PolyLine.h"

#include "SketchImage.h"

#include <vector>

using namespace std;

class CLineImage
	: public CSketchImage
{
public:
	CLineImage(int width, int height);
	~CLineImage(void);
private:
	vector<CPolyLine*> m_polyLines;
public:
	void Add(CPolyLine* polyLine);
	int Size(void);
	CPolyLine* At(int i);
	void Clear(void);
	void SolderKnots(void);
	int IsKnotInLines(CFPoint p);
	CLineImage* Clone(void);
	bool IsTail(CPolyLine* pl);
	CLineImage* SmoothPositions();
	// check all lines in image and update their tail status
	void UpdateTailData(void);
};
