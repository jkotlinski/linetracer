#include "StdAfx.h"
#include "layer.h"

#include "LayerManager.h"
#include ".\layer.h"

CLayer::CLayer()
: m_ImageProcessor(NULL)
, m_SketchImage(NULL)
, m_IsVisible(false)
, m_IsValid(false)
, m_hasBeenDrawn(false)
{
	LOG("init layer\n");
}

CLayer::~CLayer() {
	try {
		delete m_SketchImage;
		m_SketchImage=NULL;
	}
	catch (...) {
		try {
			LOG ("Caught exception!");
		}
		catch (...) {
		}
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
	m_IsVisible = state;
}

bool CLayer::IsVisible(void) const
{
	return m_IsVisible;
}

void CLayer::Process(CSketchImage *src)
{
	if(IsValid()) return;
	ASSERT ( m_ImageProcessor != NULL );

	if (m_SketchImage != NULL) 
	{
		//LOG ( "m_SketchImage: %x\n", m_SketchImage );
		delete m_SketchImage;
		m_SketchImage = NULL;
	}

	m_SketchImage = m_ImageProcessor->Process(src);

	SetValid(true);
}	

bool CLayer::IsValid(void) const
{
	return m_IsValid;
}

void CLayer::SetValid(bool isValid)
{
	m_IsValid=isValid;
	if ( m_IsValid == false )
	{
		m_hasBeenDrawn = false;
	}
}

const CString* CLayer::GetName(void)
{
	ASSERT ( m_ImageProcessor != NULL );

	const CString *l_name = m_ImageProcessor->GetName();

	return l_name;
}

int CLayer::GetImageWidth(void)
{
	CSketchImage *l_sketchImage = GetSketchImage();
	if ( l_sketchImage == NULL )
	{
		return 0;
	}
	return l_sketchImage->GetWidth();;
}

int CLayer::GetImageHeight(void)
{
	CSketchImage *l_sketchImage = GetSketchImage();
	if ( l_sketchImage == NULL )
	{
		return 0;
	}
	return l_sketchImage->GetHeight();
}

void CLayer::DrawUsingGraphics(Graphics & a_graphics)
{
	CSingleLock l_single_lock (&m_critSection);
	l_single_lock.Lock();
	CSketchImage *l_sketchImage = GetSketchImage();
	if ( l_sketchImage == NULL )
	{
		return;
	}
	l_sketchImage->DrawUsingGraphics( a_graphics );
	m_hasBeenDrawn = true;
}

bool CLayer::HasBeenDrawn(void)
{
	return m_hasBeenDrawn;
}
