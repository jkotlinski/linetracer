#include "StdAfx.h"
#include "thinner.h"

CThinner::CThinner(void)
: CImageProcessor()
{
	SetName ( CString ( "Thinner" ) );
	SetType ( THINNER );
}

CThinner::~CThinner(void)
{
}

CThinner* CThinner::Instance(void)
{
    static CThinner inst;
    return &inst;
}

CSketchImage* CThinner::Process(CSketchImage *i_src) {
	CRawImage<bool> *src=dynamic_cast<CRawImage<bool>*>(i_src);
	ASSERT ( src != NULL );
	CRawImage<bool> *dst = new CRawImage<bool>(src->GetWidth(),src->GetHeight());

	for(int i=0; i<src->GetPixels(); i++) {
		dst->SetPixel(i,!src->GetPixel(i));
	}

	int pixelsThinned = 0;
	
	do 
	{
		pixelsThinned = Thin(dst);
		//LOG("thinned: %i\n",thinned);
	} 
	while( pixelsThinned > 0 );

	return dst;
}

/*Consider all pixels on the boundaries of foreground regions. 
Delete pixel that has more than one foreground neighbor, as 
long as doing so does not locally disconnect */
int CThinner::Thin(CRawImage<bool> *img)
{
	int thinnedPixels = 0;

	deque<CPoint> pixelsToDelete;

	for(int x=1; x<img->GetWidth()-1; x++) {
		for(int y=1; y<img->GetHeight()-1; y++) {
			if(img->GetPixel(x,y)) {
				if(IsPointThinnable(img,CPoint(x,y))) {
					pixelsToDelete.push_back(CPoint(x,y));
					continue;
				}
			}
		}
	}

	while(!pixelsToDelete.empty()) {
		CPoint p = pixelsToDelete.front();
		pixelsToDelete.pop_front();

		if(IsPointThinnable(img,p)) {
			img->SetPixel(p.x, p.y, 0);
			thinnedPixels++;
		}
	}

	return thinnedPixels;
}

/*stantiford method*/
bool CThinner::IsPointThinnable(CRawImage<bool>* img, CPoint p)
{
	//0 = 0, 1 = 1, 2 = don't care
	static const char matrix[4][3][3] = 
	{
		{{2,1,2},{2,1,2},{2,0,2}},
		{{2,0,2},{2,1,2},{2,1,2}},
		{{2,2,2},{0,1,1},{2,2,2}},
		{{2,2,2},{1,1,0},{2,2,2}}
	};

	int n[9];

	n[1] = img->GetPixel(p.x+1,p.y)?1:0;
	n[2] = img->GetPixel(p.x+1,p.y+1)?1:0;
	n[3] = img->GetPixel(p.x,p.y+1)?1:0;
	n[4] = img->GetPixel(p.x-1,p.y+1)?1:0;
	n[5] = img->GetPixel(p.x-1,p.y)?1:0;
	n[6] = img->GetPixel(p.x-1,p.y-1)?1:0;
	n[7] = img->GetPixel(p.x,p.y-1)?1:0;
	n[8] = img->GetPixel(p.x+1,p.y-1)?1:0;

	int frontPixels = 0;

	for(int l_pixelIndex=1; l_pixelIndex<9; l_pixelIndex++) 
		{
		frontPixels += n[l_pixelIndex];
		}

	bool isEndPixel = frontPixels == 1;

	if(isEndPixel) return false;

	int connectivityNumber = 0;
	if(!n[1] && n[2]) connectivityNumber++;
	if(!n[2] && n[3]) connectivityNumber++;
	if(!n[3] && n[4]) connectivityNumber++;
	if(!n[4] && n[5]) connectivityNumber++;
	if(!n[5] && n[6]) connectivityNumber++;
	if(!n[6] && n[7]) connectivityNumber++;
	if(!n[7] && n[8]) connectivityNumber++;
	if(!n[8] && n[1]) connectivityNumber++;

	//LOG("connectivityNumber: %i\n",connectivityNumber);

	if(connectivityNumber!=1) return false;

	for(int m=0; m<4; m++) {

		bool doDelete=true;

		for(int i=0; doDelete && (i<3); i++) {
			for(int j=0; doDelete && (j<3); j++) {
				switch(matrix[m][i][j]) {
									case 0:
										if(img->GetPixel(p.x+i-1,p.y+j-1)) {
											doDelete=false;
										}
										break;
									case 1:
										if(!img->GetPixel(p.x+i-1,p.y+j-1)) {
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

/* runs in two passes */
bool CThinner::IsPointThinnableZhangSuen(CRawImage<bool>* img, CPoint p, int pass) {
	int n[9];

	n[1] = img->GetPixel(p.x+1,p.y)?1:0;
	n[2] = img->GetPixel(p.x+1,p.y+1)?1:0;
	n[3] = img->GetPixel(p.x,p.y+1)?1:0;
	n[4] = img->GetPixel(p.x-1,p.y+1)?1:0;
	n[5] = img->GetPixel(p.x-1,p.y)?1:0;
	n[6] = img->GetPixel(p.x-1,p.y-1)?1:0;
	n[7] = img->GetPixel(p.x,p.y-1)?1:0;
	n[8] = img->GetPixel(p.x+1,p.y-1)?1:0;

	int neighbors = 0;

	for(int i=1; i<9; i++) neighbors += n[i];

	if(neighbors<2) return false;
	if(neighbors>6) return false;

	int connectivityNumber = 0;
	if(!n[1] && n[2]) connectivityNumber++;
	if(!n[2] && n[3]) connectivityNumber++;
	if(!n[3] && n[4]) connectivityNumber++;
	if(!n[4] && n[5]) connectivityNumber++;
	if(!n[5] && n[6]) connectivityNumber++;
	if(!n[6] && n[7]) connectivityNumber++;
	if(!n[7] && n[8]) connectivityNumber++;
	if(!n[8] && n[1]) connectivityNumber++;

	if(connectivityNumber!=1) return false;

	if(pass==0) {
		if(n[1] && n[3] && n[7]) return false;
		if(n[1] && n[5] && n[7]) return false;
	} else {
		if(n[1] && n[3] && n[5]) return false;
		if(n[3] && n[5] && n[7]) return false;
	}
	return true;
}

void CThinner::PaintImage(CSketchImage* a_image, CRawImage<ARGB> *a_canvas) const
{
	static const int SCALE = 1;
	int width = a_canvas->GetWidth();
	int height = a_canvas->GetHeight();
	CRawImage<bool> *l_src = dynamic_cast<CRawImage<bool>*>(a_image);
	ASSERT ( l_src != NULL );

	for(int x=0; x<width*SCALE; x+=SCALE) {
		for(int y=0; y<height*SCALE; y+=SCALE) {
			ARGB p = l_src->GetPixel(x/SCALE,y/SCALE);

			if( p != 0 ) 
			{
				for(int i=0; i<SCALE; i++) {
					for(int j=0; j<SCALE; j++) {
						a_canvas->SetPixel(x+i, y+j, 0xff00ff00);
					}
				}
			}
		}
	}
}
