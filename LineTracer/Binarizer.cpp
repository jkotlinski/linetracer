#include "StdAfx.h"
#include ".\binarizer.h"

#include <math.h>

CBinarizer::CBinarizer(void)
: m_sketchBoard(NULL)
{
	TRACE("init binarizer\n");
	SetParam("min_threshold",-1);
	SetParam("mean_c",5);
	m_distanceMap = new CRawImage<int>(0,0);
}

CBinarizer::~CBinarizer(void)
{
	if(m_sketchBoard!=NULL) delete[] m_sketchBoard;
	delete m_distanceMap;
}

CBinarizer *CBinarizer::Instance() {
    static CBinarizer inst;
    return &inst;
}

CSketchImage* CBinarizer::Process(CSketchImage* i_src)
{
	CRawImage<unsigned char> *src=static_cast<CRawImage<unsigned char>*>(i_src);

	static const int WINDOW_SIZE = 3;
	//static const int WINDOW_SIZE = 0;

	int C = int(GetParam("mean_c"));
	int MIN_THRESHOLD = int(GetParam("min_threshold"));

	if(MIN_THRESHOLD == -1) {
		MIN_THRESHOLD = CalculateOtsuThreshold(src);
		SetParam("min_threshold",MIN_THRESHOLD);
	}

	int width = src->GetWidth();
	int height = src->GetHeight();

	if(m_sketchBoard==NULL) {
		m_sketchBoard = new int[width*height];

		for(int x=0; x<width; x++) {
			for(int y=0; y<height; y++) {
				//calculate mean threshold
				unsigned int pixelCount = 0;
				ARGB pixelVal = 0;

				for(int i=-min(x,WINDOW_SIZE); i<=WINDOW_SIZE; i++) {
					if(x+i>=0 && x+i<width) {
						for(int j=-min(y,WINDOW_SIZE); j<=WINDOW_SIZE; j++) {
							if(y+j<height) {
								int p = src->GetPixel(x+i, y+j);
								pixelCount++;
								pixelVal+=p;
							}
						}
					}
				}
				m_sketchBoard[x+y*width] = pixelVal/pixelCount;
			}
		}
	}

	CRawImage<bool> *dst=new CRawImage<bool>(width, height);

	//do adaptive thresholding
	for(int x=0; x<width; x++) {
		for(int y=0; y<height; y++) {
			//calculate mean threshold

			int c = int(src->GetPixel(x,y));
			if(c < m_sketchBoard[x+y*width]-C) {
				dst->SetPixel(x,y,0);
			} else {
				if (c < MIN_THRESHOLD) {
					dst->SetPixel(x,y,false); //green?
				} else {
					dst->SetPixel(x,y,true);
				}
			}
		}
	}

	CalcDistanceMap(dst);

	return dst;
}

int CBinarizer::CalculateOtsuThreshold(CRawImage<unsigned char> *img)
{
	//calculate histogram
	int histogram[256];
	for(int i=0; i<256; i++) histogram[i]=0;
	for(int i=0; i<img->GetHeight()*img->GetWidth(); i++) {
		histogram[img->GetPixel(i)]++;
	}

	int bestThreshold = -1;
	int bestThresholdVariance = 0xfffffff;

	for(int i=1; i<255; i++) {
		int p1 = 0;
		int p2 = 0;
		int mu1 = 0;
		int mu2 = 0;
		int v1 = 0;
		int v2 = 0;

		//calculate p1,p2
		for(int j=0; j<=i; j++) {
			p1+=histogram[j];
		}
		for(int j=i+1; j<256; j++) {
			p2+=histogram[j];
		}

		if(p1 && p2) {

			//calculate mu1, mu2
			for(int j=0; j<=i; j++) {
				mu1+=histogram[j]*j;
			}
			for(int j=i+1; j<256; j++) {
				mu2+=histogram[j]*j;
			}
			mu1/=p1;
			mu2/=p2;

			//calculate v1, v2
			for(int j=0; j<=i; j++) {
				v1+=histogram[j]*(j-mu1)*(j-mu1);
			}
			for(int j=i+1; j<256; j++) {
				v2+=histogram[j]*(j-mu2)*(j-mu2);
			}
			v1/=p1;
			v2/=p2;

			int variance = p1*v1 + p2*v2;
			if(variance<bestThresholdVariance) {
				bestThreshold = i;
				bestThresholdVariance=variance;
			}
		}
	}

	TRACE("find otsu: %i\n", bestThreshold);

	return bestThreshold;
}

void CBinarizer::Init(void)
{
	if(m_sketchBoard != NULL) {
		delete[] m_sketchBoard;
		m_sketchBoard = NULL;
	}
}

void CBinarizer::CalcDistanceMap(CRawImage<bool>* img)
{
	delete m_distanceMap;
	m_distanceMap = new CRawImage<int>(img->GetWidth(),img->GetHeight());
	m_distanceMap->Clear();
	/*for(int i=0; i<img->GetPixels(); i++) {
		if(!img->GetPixel(i)) {
			m_distanceMap->SetPixel(i,1000000);
		} else {
			m_distanceMap->SetPixel(i,0);
		}
	}*/

	for(int x=1; x<img->GetWidth()-1; x++) {
		for(int y=1; y<img->GetHeight()-1; y++) {
			if(!img->GetPixel(x,y)) {
				int newVal = m_distanceMap->GetPixel(x-1,y)+3;
				newVal = min(newVal, m_distanceMap->GetPixel(x,y-1)+3);
				newVal = min(newVal, m_distanceMap->GetPixel(x-1,y-1)+4);
				m_distanceMap->SetPixel(x,y,newVal);
			}
		}
	}
	//pass 2
	for(int x=img->GetWidth()-2; x>0; x--) {
		for(int y=img->GetHeight()-2; y>0; y--) {
			if(!img->GetPixel(x,y)) {
				int newVal = min(m_distanceMap->GetPixel(x,y),m_distanceMap->GetPixel(x+1,y)+3);
				newVal = min(newVal, m_distanceMap->GetPixel(x,y+1)+3);
				newVal = min(newVal, m_distanceMap->GetPixel(x+1,y+1)+4);
				m_distanceMap->SetPixel(x,y,newVal);
			}
		}
	}
}
