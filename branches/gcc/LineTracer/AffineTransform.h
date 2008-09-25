#pragma once

#include <wx/gdicmn.h>

class wxGraphicsMatrix;
class wxSize;

class AffineTransform
{
public:
	AffineTransform(void);

private:
	float m_translationX;
	float m_translationY;
	float m_scale;
public:
	void TranslateBy(float a_x, float a_y);
	void ScaleBy(float a_scale);
	void Invert(void);
	wxGraphicsMatrix* CreateMatrix(void);
	bool Validate(const wxSize& a_imageHeight, const wxSize& a_viewHeight);
	void TransformSize(wxSize& a_size) const;
	void TransformPoint(wxRealPoint& a_point) const;
	float GetTranslationX(void) const;
	float GetTranslationY(void) const;
};
