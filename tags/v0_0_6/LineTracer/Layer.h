#pragma once
//#include "c:\program\microsoft visual studio .net 2003\vc7\platformsdk\include\gdiplusheaders.h"

#include "ImageProcessor.h"
#include "RawImage.h"
#include "SketchImage.h"

class CLayer
{
public:
	CLayer();
	~CLayer();
private:
	CImageProcessor *m_ImageProcessor;
	CSketchImage *m_SketchImage;
	bool m_IsVisible;
	bool m_IsValid;
	Bitmap* m_cachedBitmap;
public:
	void SetImageProcessor(CImageProcessor* ImageProcessor);
public:
	CSketchImage* GetSketchImage(void);
	void SetVisible(bool state);
	bool IsVisible(void) const;
	void Process(CSketchImage *src);
public:
	bool IsValid(void) const;
	void SetValid(bool isValid);
	const CString* GetName(void);
	void PaintImage(CRawImage<ARGB>* a_canvas) const;
};
