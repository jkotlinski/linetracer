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

#include "PolyLine.h"
#include "SketchImage.h"

#include <vector>
#include <map>

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
	CPolyLine* GetLine ( unsigned int i ) const;
	void Clear(void);
	void SolderKnots(void);
	CLineImage* Clone(void) const;
	bool IsTail(map <unsigned int,int>* a_tailCounterMap, const CPolyLine* pl) const;
	CLineImage* SmoothPositions() const;
	// check all lines in image and update their tail status
	void UpdateTailData(void);
private:
	map<unsigned int,int>* GetTailPointCounterMap (void) const;
	unsigned int CalcPointKey(const CFPoint &a_point) const;
public:
	void DrawUsingGraphics(Graphics & a_graphics);
private:
	void DrawCurve(Graphics& a_graphics, CSketchPoint* a_startPoint, CSketchPoint* a_endPoint);
	void DrawLine(Graphics & a_graphics, CFPoint& a_startPoint, CFPoint& a_endPoint);
public:
	void AssertNoEmptyLines(void) const;
	void AssertNoDuplicateLines(void) const;
	void DiscardDuplicateLines(void);
};
