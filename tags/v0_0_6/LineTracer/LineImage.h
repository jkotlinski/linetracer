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
	CPolyLine* At ( unsigned int i ) const;
	void Clear(void);
	void SolderKnots(void);
	CLineImage* Clone(void) const;
	bool IsTail(map <unsigned int,int>* a_tailCounterMap, CPolyLine* pl) const;
	CLineImage* SmoothPositions() const;
	// check all lines in image and update their tail status
	void UpdateTailData(void);
private:
	map<unsigned int,int>* GetTailPointCounterMap (void) const;
	unsigned int CalcPointKey(const CFPoint &a_point) const;
};