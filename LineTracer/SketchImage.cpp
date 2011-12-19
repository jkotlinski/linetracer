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
