#pragma once

class CRawImage
{
public:
	CRawImage(int width,int height);
	~CRawImage(void);
private:
	ARGB *m_buffer;
	int m_Width;
	int m_Height;
public:
	CRawImage(Bitmap * b);
	Bitmap * MakeBitmap(void);
	void SetPixel(int x, int y, ARGB val);
	ARGB GetPixel(int x, int y);
	int GetWidth(void);
	int GetHeight(void);
};
