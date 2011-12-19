/** This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

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
	//LOG("init layer\n");
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

void CLayer::Process(CProjectSettings & a_project_settings, CSketchImage *src)
{
	if(IsValid()) return;
	ASSERT ( m_ImageProcessor != NULL );

	if (m_SketchImage != NULL) 
	{
		//LOG ( "m_SketchImage: %x\n", m_SketchImage );
		delete m_SketchImage;
		m_SketchImage = NULL;
	}

	m_SketchImage = m_ImageProcessor->Process(a_project_settings, src);

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

void CLayer::Lock() {
	m_critical_section.Lock();
}
void CLayer::Unlock() {
	m_critical_section.Unlock();
}
