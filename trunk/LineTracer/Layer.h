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
	CCriticalSection m_critical_section;

public:
	void SetImageProcessor(CImageProcessor* ImageProcessor);
public:
	CSketchImage* GetSketchImage(void);
	void SetVisible(bool state);
	bool IsVisible(void) const;
	void Process(CProjectSettings & a_project_settings, CSketchImage *src);
	void Lock();
	void Unlock();
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
};
