#pragma once

#include "PolyLine.h"

#include "SketchImage.h"

class CLineImage
	: public CSketchImage
{
public:
	CLineImage(void);
	~CLineImage(void);
private:
	vector<CPolyLine*> m_polyLines;
public:
	void Add(CPolyLine* polyLine);
	int Size(void);
	CPolyLine* At(int i);
	void Clear(void);
	void SetWidth(int width);
	void SetHeight(int height);
};
