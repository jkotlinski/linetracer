#pragma once

#include "SketchImage.h"

#include <deque>

template <class T>
class CRawImage
	: public CSketchImage
{
public:
	CRawImage(int width,int height);
	CRawImage();
	~CRawImage(void);
private:
	//copy and assignment forbidden
	//lint -e{1704}
	CRawImage (const CRawImage&);
	//lint -e{1704}
    CRawImage& operator= (const CRawImage&);

	T *m_buffer;
public:
	explicit CRawImage(Bitmap * b);
	Bitmap * GetBitmap(void) const;
	inline void SetPixel(int x, int y, T val);
	inline T GetPixel(int x, int y) const;
	inline T GetPixel(int offset) const;
	inline void SetPixel(int offset, T val);
	void Clear();
	void Dilate(void);
	void Erode(void);
	void Fill(T val = 1);

	void DrawUsingGraphics(Graphics& a_graphics);

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
	ASSERT ( m_buffer != NULL );
	m_buffer[y*GetWidth()+x]=val;
}

template <class T>
inline T CRawImage<T>::GetPixel(int x, int y) const
{
	ASSERT ( m_buffer != NULL );
 	return m_buffer[y*GetWidth()+x];
}

template <class T>
inline T CRawImage<T>::GetPixel(int offset) const
{
	ASSERT ( m_buffer != NULL );
	return m_buffer[offset];
}

template <class T>
inline void CRawImage<T>::SetPixel(int offset, T val)
{
	ASSERT ( m_buffer != NULL );
	m_buffer[offset]=val;
}

#include <vector>

template <class T>
CRawImage<T>::CRawImage()
: CSketchImage()
, m_buffer(NULL)
{
	SetSize(0,0);
	m_buffer=new T[0];
}

template <class T>
CRawImage<T>::CRawImage(int width, int height)
: CSketchImage()
, m_buffer(NULL)
{
	SetSize(width,height);
	ASSERT ( width>=0 );
	ASSERT ( height>=0 );
	//lint -e{737} loss of sign ok
	m_buffer = new T[width*height];
}

template <class T>
CRawImage<T>::~CRawImage(void)
{
	if(m_buffer!=NULL) delete m_buffer;
}

template <class T>
CRawImage<T>::CRawImage(Bitmap * b)
: CSketchImage()
, m_buffer(NULL)
{
	SetSize( int(b->GetWidth()), int(b->GetHeight()) );

	if (GetPixels()<=0) return;

	// allocate memory for pixel data
	//lint -e{737} sign loss ok
	m_buffer = new T[GetPixels()];

	if ( m_buffer == NULL )
		return;

	// get the pixel values from the bitmap and encode
	// into the array allocated above.

	BitmapData bmData;
	Rect rect(0, 0, GetWidth(), GetHeight());

	Status l_result = b->LockBits(&rect,
		ImageLockModeRead,
		PixelFormat32bppARGB,
		&bmData);
	ASSERT ( l_result == Ok );

	int stride = bmData.Stride;
	BYTE *p = static_cast<BYTE*> ( static_cast<void*>(bmData.Scan0));

	int nOffset = stride - GetWidth()*4;    // bytes to skip at end of
	// each row

	for(int y=0; y < GetHeight();y++) {
		for(int x=0; x < GetWidth(); ++x ) {
			// GDI lies about RGB - internally it's BGR
			m_buffer[y*GetWidth()+x] =  static_cast<T> ( p[2]<<16 | p[1]<<8 | p[0] );
			p += 4;
		}
		p += nOffset;
	}
	Status l_unlockResult = b->UnlockBits(&bmData);
	ASSERT ( l_unlockResult == Ok );
}

template <class T>
Bitmap * CRawImage<T>::GetBitmap(void) const
{
	// create a temporary Bitmap
	Bitmap bit(GetWidth(), GetHeight(), PixelFormat32bppARGB);

	// create its clone for returning
	Bitmap *b = bit.Clone(0, 0, int(bit.GetWidth()), int(bit.GetHeight()),
		PixelFormat32bppARGB);

	BitmapData bmData;
	Rect rect(0, 0, int(b->GetWidth()), int(b->GetHeight()) );
	Status l_result = b->LockBits(&rect,
		ImageLockModeRead | ImageLockModeWrite,
		PixelFormat32bppARGB,
		&bmData);
	ASSERT ( l_result == Ok);

	int stride = bmData.Stride;
	BYTE *p = static_cast<BYTE*> (static_cast<void*> (bmData.Scan0));
	int nOffset = stride - GetWidth()*4;   // bytes to skip at end of
	// each row

	ARGB pixel;

	for(int y=0; y < GetHeight();y++) {
		for(int x=0; x < GetWidth(); ++x ) {
			// GDI lies about RGB - internally it's BGR
			//lint -e{732} sign loss OK
			pixel = m_buffer[y*GetWidth()+x];
			p[3] = static_cast<BYTE> ((pixel >> 24) & 0xff);    // pixel alpha
			p[2] = static_cast<BYTE> ((pixel >> 16) & 0xff);    // pixel red
			p[1] = static_cast<BYTE> ((pixel >> 8 ) & 0xff);    // pixel green
			p[0] = static_cast<BYTE> ((pixel      ) & 0xff);    // pixel blue
			p += 4;
		}
		p += nOffset;
	}
	Status l_unlockResult = b->UnlockBits(&bmData);
	ASSERT ( l_unlockResult == Ok );

	return b;
}

template <class T>
void CRawImage<T>::Clear(void)
{
	for(int i=0; i<GetPixels(); i++) {
		SetPixel(i,static_cast<T> ( 0 ));
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
	for(iter=tmp2.begin(); iter!=tmp2.end(); ++iter) {
		CPoint p = (*iter);
		SetPixel(p.x,p.y, static_cast<T> ( 0 ));
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
	for(iter=tmp2.begin(); iter!=tmp2.end(); ++iter) {
		CPoint p = (*iter);
		SetPixel(p.x,p.y, static_cast<T>(0xffffff));
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

template <class T>
void CRawImage<T>::DrawUsingGraphics(Graphics& a_graphics)
{
	CLogger::Activate();
	LOG ( "RawImage->DrawUsingGraphics()\n" );

	Bitmap l_bitmap ( GetWidth(), GetHeight(), PixelFormat32bppARGB );

	for ( int x=0; x<GetWidth(); x++ )
	{
		for ( int y=0; y<GetHeight(); y++ )
		{
			if ( GetPixel( x, y ) == 0 )
			{
				(void) l_bitmap.SetPixel ( x, y, Color::Black );
			}
			else
			{
				(void) l_bitmap.SetPixel ( x, y, Color::White );
			}
		}
	}

	Status l_drawImageResult = a_graphics.DrawImage( &l_bitmap, 0, 0, GetWidth(), GetHeight());
	ASSERT ( l_drawImageResult == Ok );
}
