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
