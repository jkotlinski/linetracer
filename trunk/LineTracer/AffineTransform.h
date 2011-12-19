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

class AffineTransform
{
public:
	AffineTransform(void);
	~AffineTransform(void);
private:
	float m_translationX;
	float m_translationY;
	float m_scale;
public:
	void TranslateBy(float a_x, float a_y);
	void ScaleBy(float a_scale);
	void Invert(void);
	Matrix *GetMatrix(void);
	bool Validate(const CSize & a_imageHeight, const CSize & a_viewHeight);
	void TransformSize(CSize & a_size) const;
	void TransformPoint(PointF & a_point) const;
	float GetTranslationX(void) const;
	float GetTranslationY(void) const;
};
