#pragma once

class CSketchImage
{
public:
	CSketchImage(void);
	virtual ~CSketchImage(void);

	int GetWidth(void);
	int GetHeight(void);
	int GetPixels(void);
protected:
	int m_Width;
	int m_Height;
	int m_Pixels;
public:
	virtual void Clear(void)=0;
};

inline int CSketchImage::GetHeight() {
	return m_Height;
}

inline int CSketchImage::GetWidth() {
	return m_Width;
}

inline int CSketchImage::GetPixels() {
	return m_Pixels;
}