#pragma once

#include "SketchImage.h"

class CRawImage
	: public CSketchImage
{
public:
	CRawImage(int width,int height);
	~CRawImage(void);
private:
	ARGB *m_buffer;
public:
	CRawImage(Bitmap * b);
	Bitmap * MakeBitmap(void);
	void SetPixel(int x, int y, ARGB val);
	ARGB GetPixel(int x, int y);
	ARGB GetPixel(int offset);
	void SetPixel(int offset, ARGB val);
	void Clear();
};
