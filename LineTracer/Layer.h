#pragma once
//#include "c:\program\microsoft visual studio .net 2003\vc7\platformsdk\include\gdiplusheaders.h"

#include "ImageProcessor.h"
#include "RawImage.h"
#include "SketchImage.h"

#include <afxmt.h>

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
	int GetImageWidth(void);
	int GetImageHeight(void);
	void DrawUsingGraphics(Graphics & a_graphics);
private:
	bool m_hasBeenDrawn;
public:
	bool HasBeenDrawn(void);

private:
	CCriticalSection m_critSection;
};
