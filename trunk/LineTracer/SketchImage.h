#pragma once

class CSketchImage
{
public:
	CSketchImage(void);
	~CSketchImage(void);

	int GetWidth(void);
	int GetHeight(void);
protected:
	int m_Width;
	int m_Height;
public:
	virtual void Clear(void)=0;
};
