#include "StdAfx.h"
#include ".\rawimage.h"

CRawImage::CRawImage(int width, int height)
: m_buffer(NULL)
{
	m_Width=width;
	m_Height=height;

	m_buffer=new ARGB[width*height];
}

CRawImage::~CRawImage(void)
{
	if(m_buffer!=NULL) delete m_buffer;
}

void CRawImage::SetPixel(int x, int y, ARGB val)
{
	m_buffer[y*m_Width+x]=val;
}

ARGB CRawImage::GetPixel(int x, int y)
{
	return m_buffer[y*m_Width+x];
}

CRawImage::CRawImage(Bitmap * b)
{
	m_Width = b->GetWidth();
	m_Height = b->GetHeight();

	if (m_Width*m_Height<=0) return;

	// allocate memory for pixel data
	m_buffer = new ARGB[m_Width*m_Height];

	if ( m_buffer == NULL )
		return;

	// get the pixel values from the bitmap and encode
	// into the array allocated above.

	BitmapData bmData;
	Rect rect(0, 0, m_Width, m_Height);

	b->LockBits(&rect,
		ImageLockModeRead,
		PixelFormat32bppARGB,
		&bmData);

	int stride = bmData.Stride;
	BYTE *p = (BYTE *)((void *)bmData.Scan0);

	int nOffset = stride - m_Width*4;    // bytes to skip at end of
	// each row

	for(int y=0; y < m_Height;y++) {
		for(int x=0; x < m_Width; ++x ) {
			// GDI lies about RGB - internally it's BGR
			m_buffer[y*m_Width+x] =  p[2]<<16 | p[1]<<8 | p[0];
			p += 4;
		}
		p += nOffset;
	}
	b->UnlockBits(&bmData);
}

Bitmap * CRawImage::MakeBitmap(void)
{
	// create a temporary Bitmap
	Bitmap bit(m_Width, m_Height, PixelFormat32bppARGB);

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
	int nOffset = stride - m_Width*4;   // bytes to skip at end of
	// each row

	int pixel;

	for(int y=0; y <m_Height;y++) {
		for(int x=0; x < m_Width; ++x ) {
			// GDI lies about RGB - internally it's BGR
			pixel = m_buffer[y*m_Width+x];
			p[3] = 0xff;    // alpha
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


int CRawImage::GetWidth(void)
{
	return m_Width;
}

int CRawImage::GetHeight(void)
{
	return m_Height;
}
