#pragma once

#include "SketchImage.h"

#include <deque>
#include <assert.h>

#include <wx/bitmap.h>
#include <wx/gdicmn.h>
#include <wx/graphics.h>
#include <wx/rawbmp.h>

typedef wxPixelData<wxBitmap, wxNativePixelFormat> PixelData;

class wxBitmap;
class wxGraphicsContext;

using namespace std;

static const bool k_black = false;
static const bool k_white = true;

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
	explicit CRawImage(wxBitmap * b);
	wxBitmap * GetBitmap(void) const;
	inline void SetPixel(int x, int y, T val);
	inline T GetPixel(int x, int y) const;
	inline T GetPixel(int offset) const;
	inline void SetPixel(int offset, T val);
	void Clear();
	void PaintPixels( deque<pair<int,int> > a_pixels_to_paint, T a_color );
	void Dilate(void);
	void Erode(void);
	void Fill(T val = 1);
	CRawImage<T> *Clone() const;

	void DrawUsingGraphics(wxGraphicsContext& a_graphics);

private:

	bool IsPointThinnable(wxPoint p) {
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

public:
	void calculate_histogram(int * a_histogram, int a_levels) const
	{
		memset ( a_histogram, 0, a_levels );
		for ( int l_offset = 0;
			l_offset < GetPixelCount();
			l_offset++ )
		{
			int l_level = GetPixel(l_offset);
			assert ( l_level < a_levels || !"histogram: intensity out of allowed range" );
			a_histogram[l_level]++;
		}
	}
};

template <class T>
inline void CRawImage<T>::SetPixel(int x, int y, T val)
{
	wxASSERT ( m_buffer != NULL );
	m_buffer[y*GetWidth()+x]=val;
}

template <class T>
inline T CRawImage<T>::GetPixel(int x, int y) const
{
	wxASSERT ( m_buffer != NULL );
 	return m_buffer[y*GetWidth()+x];
}

template <class T>
inline T CRawImage<T>::GetPixel(int offset) const
{
	wxASSERT ( m_buffer != NULL );
	return m_buffer[offset];
}

template <class T>
inline void CRawImage<T>::SetPixel(int offset, T val)
{
	wxASSERT ( m_buffer != NULL );
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
	wxASSERT ( width>=0 );
	wxASSERT ( height>=0 );
	//lint -e{737} loss of sign ok
	m_buffer = new T[width*height];
}

template <class T>
CRawImage<T>::~CRawImage(void)
{
	if(m_buffer!=NULL) delete m_buffer;
}

template <class T>
CRawImage<T>::CRawImage(wxBitmap * b)
: CSketchImage()
, m_buffer(NULL)
{
	SetSize( int(b->GetWidth()), int(b->GetHeight()) );

	if (GetPixelCount()<=0) return;

	// allocate memory for pixel data
	//lint -e{737} sign loss ok
	m_buffer = new T[GetPixelCount()];

	if ( m_buffer == NULL )
		return;

	// get the pixel values from the bitmap and encode
	// into the array allocated above.

	PixelData pixels(b);

	wxASSERT ( pixels );

	PixelData::Iterator srcIt ( pixels );

	for(int dstIt = 0; dstIt < GetWidth() * GetHeight(); ++dstIt) 
	{
		m_buffer[dstIt] =  static_cast<T> ( srcIt.Red() << 16 | srcIt.Green() << 8 | srcIt.Blue() );
		++srcIt;
	}
}

template <class T>
wxBitmap * CRawImage<T>::GetBitmap(void) const
{
	wxBitmap *b = new wxBitmap ( GetWidth(), GetHeight(), 32 );

	PixelData pixels ( *b );
	wxASSERT ( pixels );

	PixelData::Iterator dstIt ( pixels );

	for(int srcIt = 0; srcIt < GetWidth() * GetHeight(); ++srcIt) 
	{
		const unsigned int pixel = m_buffer[srcIt] ? 0xffffffff : 0xff000000;
		dstIt.Alpha() = (pixel >> 24) & 0xff;
		dstIt.Red() = (pixel >> 16) & 0xff;
		dstIt.Green() = (pixel >> 8) & 0xff;
		dstIt.Blue() = pixel & 0xff;
	}

	return b;
}

template <class T>
void CRawImage<T>::Clear(void)
{
	for(int i=0; i<GetPixelCount(); i++) {
		SetPixel(i,static_cast<T> ( 0 ));
	}
}

template <class T>
void CRawImage<T>::Dilate(void)
{
	std::vector<wxPoint> tmp2;
	for(int x=1; x<GetWidth()-1; x++) {
		for(int y=1; y<GetHeight()-1; y++) {
			if(GetPixel(x,y)) {
				if(!GetPixel(x-1,y) || !GetPixel(x+1,y) || !GetPixel(x,y-1) || !GetPixel(x,y+1) ||
					!GetPixel(x-1,y-1) || !GetPixel(x+1,y-1) || !GetPixel(x+1,y+1) || !GetPixel(x-1,y+1)) {
					tmp2.push_back(wxPoint(x,y));
				}
			}
		}
	}
	std::vector<wxPoint>::iterator iter;
	for(iter=tmp2.begin(); iter!=tmp2.end(); ++iter) {
		wxPoint p = (*iter);
		SetPixel(p.x,p.y, static_cast<T> ( 0 ));
	}
	tmp2.clear();
}

template <class T>
void CRawImage<T>::Erode(void)
{
	std::vector<wxPoint> tmp2;
	for(int x=1; x<GetWidth()-1; x++) {
		for(int y=1; y<GetHeight()-1; y++) {
			if(!GetPixel(x,y)) {
				if(GetPixel(x-1,y) || GetPixel(x+1,y) || GetPixel(x,y-1) || GetPixel(x,y+1) ||
					GetPixel(x-1,y-1) || GetPixel(x+1,y-1) || GetPixel(x+1,y+1) || GetPixel(x-1,y+1)
					) {
					tmp2.push_back(wxPoint(x,y));
				}
			}
		}
	}
	std::vector<wxPoint>::iterator iter;
	for(iter=tmp2.begin(); iter!=tmp2.end(); ++iter) {
		wxPoint p = (*iter);
		SetPixel(p.x,p.y, static_cast<T>(0xffffff));
	}
	tmp2.clear();
}

template <class T>
void CRawImage<T>::Fill(T val)
{
	for(int i=0; i<GetPixelCount(); i++) {
		SetPixel(i,val);
	}
}

template <class T>
void CRawImage<T>::PaintPixels( deque<pair<int,int> > a_pixels_to_paint, T a_color )
{
	while ( !a_pixels_to_paint.empty() )
	{
		const pair<int,int> l_pixelPos = a_pixels_to_paint.front();
		a_pixels_to_paint.pop_front();
		SetPixel(l_pixelPos.first, l_pixelPos.second, a_color);
	}
}

template <class T>
void CRawImage<T>::DrawUsingGraphics(wxGraphicsContext& a_graphics)
{
	//CLogger::Activate();
	//LOG ( "RawImage->DrawUsingGraphics()\n" );

	wxBitmap *l_bitmap = GetBitmap();
	a_graphics.DrawBitmap( *l_bitmap, 0, 0, GetWidth(), GetHeight());
	delete l_bitmap;
}

template <class T>
CRawImage<T>* CRawImage<T>::Clone()
const
{
	CRawImage<T> *l_clone = new CRawImage<T>(GetWidth(), GetHeight());
	for ( int i=0; i<GetWidth()*GetHeight(); i++) 
	{
		l_clone->SetPixel( i, GetPixel(i) );
	}
	return l_clone;
}
