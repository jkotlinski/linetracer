#include "StdAfx.h"
#include "layer.h"

#include "LayerManager.h"

CLayer::CLayer()
: m_ImageProcessor(NULL)
, m_SketchImage(NULL)
, m_IsVisible(false)
, m_IsValid(false)
, m_cachedBitmap(NULL)
{
	LOG("init layer\n");
}

CLayer::~CLayer() {
	try {
		if(m_SketchImage!=NULL) {
			delete m_SketchImage;
			m_SketchImage=NULL;
		}
		if(m_cachedBitmap!=NULL) {
			delete m_cachedBitmap;
			m_cachedBitmap=NULL;
		}
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
	if ( state != m_IsVisible )
	{
		//visibility state changed! tell layer manager
		CLayerManager *l_lm = CLayerManager::Instance();
		l_lm->ChangedLayerVisibleState();
	}
	m_IsVisible=state;
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
}

const CString* CLayer::GetName(void)
{
	ASSERT ( m_ImageProcessor != NULL );

	const CString *l_name = m_ImageProcessor->GetName();

	return l_name;
}

void CLayer::PaintImage(CRawImage<ARGB>* a_canvas) const
{
	ASSERT ( a_canvas != NULL );
	ASSERT ( m_ImageProcessor != NULL );

	m_ImageProcessor->PaintImage ( m_SketchImage, a_canvas );
}
