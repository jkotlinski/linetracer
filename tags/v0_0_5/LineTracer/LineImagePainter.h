#pragma once

#include "RawImage.h"
#include "LineImage.h"

class CLineImagePainter
{
public:
	static void Paint(CRawImage<ARGB>* canvas, const CLineImage* li);
private:
	static void DrawLine(CRawImage<ARGB>* canvas, const CFPoint* start, const CFPoint* end, ARGB c);
	static void DrawPoint(CRawImage<ARGB>* canvas, const CFPoint* p, ARGB color);
	static void DrawCurve(CRawImage<ARGB>* canvas, const CSketchPoint* start, const CSketchPoint* end, ARGB color);
};
