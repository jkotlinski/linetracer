#pragma once

#include <vector>

#include "SketchPoint.h"

using namespace std;

class CPolyLine
{
public:
	CPolyLine(void);
	~CPolyLine(void);
	void Add(CSketchPoint point);
private:
	vector<CSketchPoint> m_points;
public:
	bool Contains(CPoint p);
	int Size(void);
	CSketchPoint At(int i);
	bool IsTail(void);
};
