#include "StdAfx.h"
#include ".\lineimage.h"

#include "PolyLine.h"

#include <vector>
#include <math.h>

CLineImage::CLineImage(void)
{
}

CLineImage::~CLineImage(void)
{
	for(unsigned int i=0; i<m_polyLines.size(); i++) {
		delete m_polyLines.at(i);
	}
}

void CLineImage::Add(CPolyLine* polyLine)
{
	m_polyLines.push_back(polyLine);
}

int CLineImage::Size(void)
{
	return (int)m_polyLines.size();
}

CPolyLine* CLineImage::At(int i)
{
	return m_polyLines.at(i);
}


void CLineImage::Clear(void)
{
	for(unsigned int i=0; i<m_polyLines.size(); i++) {
		delete m_polyLines.at(i);
	}
	m_polyLines.clear();
}

void CLineImage::SetSize(int width, int height)
{
	m_Width=width;
	m_Height=height;
	m_Pixels = width*height;
}