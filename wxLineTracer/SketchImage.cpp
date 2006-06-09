#include "StdAfx.h"
#include "sketchimage.h"

CSketchImage::CSketchImage(void)
: m_Width(0)
, m_Height(0)
, m_Pixels(0)
{
		//LOG("init sketchimage\n");
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
