#pragma once

class AffineTransform
{
public:
	AffineTransform(void);
	~AffineTransform(void);
public:
	float m_translationX;
	float m_translationY;
	float m_scale;
public:
	void TranslateBy(float a_x, float a_y);
	void ScaleBy(float a_scale);
	void Invert(void);
	Matrix *GetMatrix(void);
	bool Validate(CSize a_imageHeight, CSize a_viewHeight);
	void TransformSize(CSize & a_size);
	void TransformPoint(PointF & a_point);
};
