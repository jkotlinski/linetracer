#pragma once

class CSketchImage
{
public:
	CSketchImage(void);
	virtual ~CSketchImage(void);

	void SetSize(int width, int height);
	int GetWidth(void) const;
	int GetHeight(void) const;
	int GetPixels(void) const;
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

inline int CSketchImage::GetPixels() const {
	return m_Pixels;
}

