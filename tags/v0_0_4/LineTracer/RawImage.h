#pragma once

#include "SketchImage.h"

//#define __WIN32__
//#include <boost/pool/pool_alloc.hpp>

#include <deque>

template <class T>
class CRawImage
	: public CSketchImage
{
public:
	CRawImage(int width,int height);
	~CRawImage(void);
private:
	T *m_buffer;
public:
	CRawImage(Bitmap * b);
	Bitmap * MakeBitmap(void);
	void SetPixel(int x, int y, T val);
	T GetPixel(int x, int y);
	T GetPixel(int offset);
	void SetPixel(int offset, T val);
	void Clear();
	void Dilate(void);
	void Erode(void);
	void Fill(T val = 1);

private:

	bool IsPointThinnable(CPoint p) {
	//0 = 0, 1 = 1, 2 = don't care
	static const char matrix[8][3][3] = 
	{
		{{0,0,0},{2,1,2},{1,1,1}},
		{{2,0,0},{1,1,0},{2,1,2}},
		{{1,2,0},{1,1,0},{1,2,0}},
		{{2,1,2},{1,1,0},{2,0,0}},
		{{1,1,1},{2,1,2},{0,0,0}},
		{{2,1,2},{0,1,1},{0,0,2}},
		{{0,2,1},{0,1,1},{0,2,1}},
		{{0,0,2},{0,1,1},{2,1,2}}
	};

	for(int m=0; m<8; m++) {

		bool doDelete=true;

		for(int i=0; doDelete && (i<3); i++) {
			for(int j=0; doDelete && (j<3); j++) {
				switch(matrix[m][i][j]) {
									case 0:
										if(GetPixel(p.x+i-1,p.y+j-1)) {
											doDelete=false;
										}
										break;
									case 1:
										if(!GetPixel(p.x+i-1,p.y+j-1)) {
											doDelete=false;
										}
										break;
				}
			}
		}

		if(doDelete) {
			return true;
		}
	}
	return false;
}

};

template <class T>
inline void CRawImage<T>::SetPixel(int x, int y, T val)
{
	m_buffer[y*GetWidth()+x]=val;
}

template <class T>
inline T CRawImage<T>::GetPixel(int x, int y)
{
	return m_buffer[y*GetWidth()+x];
}

template <class T>
inline T CRawImage<T>::GetPixel(int offset)
{
	return m_buffer[offset];
}

template <class T>
inline void CRawImage<T>::SetPixel(int offset, T val)
{
	m_buffer[offset]=val;
}

#include <vector>

template <class T>
CRawImage<T>::CRawImage(int width, int height)
{
	SetSize(width,height);
	m_buffer=new T[width*height];
}

template <class T>
CRawImage<T>::~CRawImage(void)
{
	if(m_buffer!=NULL) delete m_buffer;
}

template <class T>
CRawImage<T>::CRawImage(Bitmap * b)
{
	SetSize(b->GetWidth(),b->GetHeight());

	if (GetPixels()<=0) return;

	// allocate memory for pixel data
	m_buffer = new T[GetPixels()];

	if ( m_buffer == NULL )
		return;

	// get the pixel values from the bitmap and encode
	// into the array allocated above.

	BitmapData bmData;
	Rect rect(0, 0, GetWidth(), GetHeight());

	b->LockBits(&rect,
		ImageLockModeRead,
		PixelFormat32bppARGB,
		&bmData);

	int stride = bmData.Stride;
	BYTE *p = (BYTE *)((void *)bmData.Scan0);

	int nOffset = stride - GetWidth()*4;    // bytes to skip at end of
	// each row

	for(int y=0; y < GetHeight();y++) {
		for(int x=0; x < GetWidth(); ++x ) {
			// GDI lies about RGB - internally it's BGR
			m_buffer[y*GetWidth()+x] =  p[2]<<16 | p[1]<<8 | p[0];
			p += 4;
		}
		p += nOffset;
	}
	b->UnlockBits(&bmData);
}

template <class T>
Bitmap * CRawImage<T>::MakeBitmap(void)
{
	// create a temporary Bitmap
	Bitmap bit(GetWidth(), GetHeight(), PixelFormat32bppARGB);

	// create its clone for returning
	Bitmap *b = bit.Clone(0, 0, bit.GetWidth(), bit.GetHeight(),
		PixelFormat32bppARGB);

	BitmapData bmData;
	Rect rect(0, 0, b->GetWidth(), b->GetHeight());
	b->LockBits(&rect,
		ImageLockModeRead | ImageLockModeWrite,
		PixelFormat32bppARGB,
		&bmData);

	int stride = bmData.Stride;
	BYTE *p = (BYTE *)((void *)bmData.Scan0);
	int nOffset = stride - GetWidth()*4;   // bytes to skip at end of
	// each row

	int pixel;

	for(int y=0; y < GetHeight();y++) {
		for(int x=0; x < GetWidth(); ++x ) {
			// GDI lies about RGB - internally it's BGR
			pixel = m_buffer[y*GetWidth()+x];
			p[3] = (BYTE) ((pixel >> 24) & 0xff);    // pixel alpha
			p[2] = (BYTE) ((pixel >> 16) & 0xff);    // pixel red
			p[1] = (BYTE) ((pixel >> 8 ) & 0xff);    // pixel green
			p[0] = (BYTE) ((pixel      ) & 0xff);    // pixel blue
			p += 4;
		}
		p += nOffset;
	}
	b->UnlockBits(&bmData);

	return b;
}

template <class T>
void CRawImage<T>::Clear(void)
{
	for(int i=0; i<GetPixels(); i++) {
		SetPixel(i,0);
	}
}

template <class T>
void CRawImage<T>::Dilate(void)
{
	std::vector<CPoint> tmp2;
	for(int x=1; x<GetWidth()-1; x++) {
		for(int y=1; y<GetHeight()-1; y++) {
			if(GetPixel(x,y)) {
				if(!GetPixel(x-1,y) || !GetPixel(x+1,y) || !GetPixel(x,y-1) || !GetPixel(x,y+1)) {
					tmp2.push_back(CPoint(x,y));
				}
			}
		}
	}
	std::vector<CPoint>::iterator iter;
	for(iter=tmp2.begin(); iter!=tmp2.end(); iter++) {
		CPoint p = (*iter);
		SetPixel(p.x,p.y,0);
	}
	tmp2.clear();
}

template <class T>
void CRawImage<T>::Erode(void)
{
	std::vector<CPoint> tmp2;
	for(int x=1; x<GetWidth()-1; x++) {
		for(int y=1; y<GetHeight()-1; y++) {
			if(!GetPixel(x,y)) {
				if(GetPixel(x-1,y) || GetPixel(x+1,y) || GetPixel(x,y-1) || GetPixel(x,y+1)) {
					tmp2.push_back(CPoint(x,y));
				}
			}
		}
	}
	std::vector<CPoint>::iterator iter;
	for(iter=tmp2.begin(); iter!=tmp2.end(); iter++) {
		CPoint p = (*iter);
		SetPixel(p.x,p.y,0xffffff);
	}
	tmp2.clear();
}

template <class T>
void CRawImage<T>::Fill(T val)
{
	for(int i=0; i<GetPixels(); i++) {
		SetPixel(i,val);
	}
}

