#pragma once
#include "c:\program\microsoft visual studio .net 2003\vc7\platformsdk\include\gdiplusheaders.h"

#include "ImageProcessor.h"
#include "RawImage.h"

class CLayer
{
public:
	CLayer(void);
	~CLayer(void);
private:
	CImageProcessor * m_ImageProcessor;
public:
	void SetImageProcessor(CImageProcessor* ImageProcessor);
private:
	CRawImage *m_RawImage;
	bool m_IsVisible;
public:
	CRawImage *GetRawImage(void);
	void SetVisible(bool state);
	bool IsVisible(void);
	void Process(CRawImage *src);
private:
	bool m_IsValid;
public:
	bool IsValid(void);
	void SetValid(bool isValid);
};
