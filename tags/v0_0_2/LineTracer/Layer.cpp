#include "StdAfx.h"
#include ".\layer.h"

CLayer::CLayer(void)
: m_ImageProcessor(NULL)
, m_IsVisible(false)
, m_SketchImage(NULL)
, m_IsValid(false)
{
}

CLayer::~CLayer(void)
{
	if(m_SketchImage!=NULL) {
		delete m_SketchImage;
		m_SketchImage=NULL;
	}
}

void CLayer::SetImageProcessor(CImageProcessor* ImageProcessor)
{
	m_ImageProcessor=ImageProcessor;
}

CSketchImage * CLayer::GetSketchImage(void)
{
	return m_SketchImage;
}

void CLayer::SetVisible(bool state)
{
	m_IsVisible=state;
}

bool CLayer::IsVisible(void)
{
	return m_IsVisible;
}

void CLayer::Process(CSketchImage *src)
{
	if(IsValid()) return;

	if(m_SketchImage!=NULL) {
		delete m_SketchImage;
		m_SketchImage=NULL;
	}

	m_SketchImage=m_ImageProcessor->Process(src);

	SetValid(true);
}

bool CLayer::IsValid(void)
{
	return m_IsValid;
}

void CLayer::SetValid(bool isValid)
{
	m_IsValid=isValid;
}
