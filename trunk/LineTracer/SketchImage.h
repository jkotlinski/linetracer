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

class CSketchImage
{
public:
	CSketchImage(void);
	virtual ~CSketchImage(void);

	void SetSize(int width, int height);
	int GetWidth(void) const;
	int GetHeight(void) const;
	int GetPixelCount(void) const;
private:
	int m_Width;
	int m_Height;
	int m_Pixels;
public:
	virtual void Clear(void)=0;
	virtual void DrawUsingGraphics(Graphics & a_graphics)=0;
};

inline int CSketchImage::GetHeight() const {
	return m_Height;
}

inline int CSketchImage::GetWidth() const {
	return m_Width;
}

inline int CSketchImage::GetPixelCount() const {
	return m_Pixels;
}

