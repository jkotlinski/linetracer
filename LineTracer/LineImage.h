#pragma once

#include "PolyLine.h"

#include "SketchImage.h"

#include <vector>

using namespace std;

class CLineImage
	: public CSketchImage
{
public:
	CLineImage(void);
	CLineImage(int width, int height);
	~CLineImage(void);
private:
	vector<CPolyLine*> m_polyLines;
public:
	void Add(CPolyLine* polyLine);
	const unsigned int Size(void) const;
	CPolyLine* At ( unsigned int i ) const;
	void Clear(void);
	void SolderKnots(void);
	const int IsKnotInLines(const CFPoint &p) const;
	CLineImage* Clone(void) const;
	bool IsTail(CPolyLine* pl) const;
	CLineImage* SmoothPositions() const;
	// check all lines in image and update their tail status
	void UpdateTailData(void);
};
