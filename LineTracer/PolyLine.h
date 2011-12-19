/** This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

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
	CSketchPoint* GetHeadPoint(unsigned int a_offset=0) const;
	CSketchPoint* GetTailPoint(unsigned int a_offset=0) const;
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
	void Flip(void);
};
