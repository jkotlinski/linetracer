#include "StdAfx.h"
#include ".\sketchimage.h"

CSketchImage::CSketchImage(void)
{
		TRACE("init sketchimage\n");
}

CSketchImage::~CSketchImage(void)
{
}

void CSketchImage::SetSize(int width, int height)
{
	m_Width=width;
	m_Height=height;
	m_Pixels = width*height;
}
