#include "StdAfx.h"
#include ".\layer.h"

CLayer::CLayer(void)
: m_ImageProcessor(NULL)
, m_IsVisible(false)
, m_RawImage(NULL)
, m_IsValid(false)
{
}

CLayer::~CLayer(void)
{
	if(m_ImageProcessor!=NULL) delete m_ImageProcessor;
	if(m_RawImage!=NULL) delete m_RawImage;
}

void CLayer::SetImageProcessor(CImageProcessor* ImageProcessor)
{
	m_ImageProcessor=ImageProcessor;
}

CRawImage * CLayer::GetRawImage(void)
{
	return m_RawImage;
}

void CLayer::SetVisible(bool state)
{
	m_IsVisible=state;
}

bool CLayer::IsVisible(void)
{
	return m_IsVisible;
}

void CLayer::Process(CRawImage *src)
{
	if(IsValid()) return;

	if(m_RawImage!=NULL) delete m_RawImage;

	m_RawImage=m_ImageProcessor->Process(src);

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
