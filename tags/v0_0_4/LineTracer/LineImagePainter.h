#pragma once

#include "RawImage.h"
#include "LineImage.h"

class CLineImagePainter
{
public:
	static void Paint(CRawImage<ARGB>* canvas, CLineImage* li);
private:
	static void DrawLine(CRawImage<ARGB>* canvas, CFPoint* start, CFPoint* end, ARGB c);
	static void DrawPoint(CRawImage<ARGB>* canvas, CFPoint* p, ARGB color);
	static void DrawCurve(CRawImage<ARGB>* canvas, CSketchPoint* start, CSketchPoint* end, ARGB color);
};
