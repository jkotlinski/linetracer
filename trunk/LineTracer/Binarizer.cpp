#include "StdAfx.h"
#include "binarizer.h"

#include "ProjectSettings.h"
#include ".\binarizer.h"

CBinarizer::CBinarizer(void)
: CImageProcessor()
, m_sketchBoard(NULL)
, m_distanceMap()
, m_isInitialized(false)
{
	LOG("init binarizer\n");
	SetName(CString("binarizer"));
	SetType ( BINARIZER );
}

CBinarizer::~CBinarizer(void)
{
	delete[] m_sketchBoard;
	m_sketchBoard = NULL;
	delete m_distanceMap;
}

CBinarizer *CBinarizer::Instance() {
    static CBinarizer inst;
    return &inst;
}

CSketchImage* CBinarizer::Process(CSketchImage* i_src)
{
	CRawImage<unsigned char> *src=dynamic_cast<CRawImage<unsigned char>*>(i_src);
	ASSERT ( src != NULL );

	static const int WINDOW_SIZE = 3;
	//static const int WINDOW_SIZE = 0;

	CProjectSettings *l_settings = CProjectSettings::Instance();
	int C = int(l_settings->GetParam(CProjectSettings::BINARIZER_MEAN_C));

	double MIN_THRESHOLD = 0.0;

	if( m_isInitialized ) 
	{
		// load threshold from settings
		MIN_THRESHOLD = l_settings->GetParam(CProjectSettings::BINARIZER_THRESHOLD);
	}
	else
	{
		// calculate good threshold
		MIN_THRESHOLD = CalculateOtsuThreshold(src);
		l_settings->SetParam(CProjectSettings::BINARIZER_THRESHOLD, MIN_THRESHOLD);
		m_isInitialized = true;
	}

	ASSERT ( src!=NULL );
	int width = src->GetWidth();
	int height = src->GetHeight();

	if(m_sketchBoard == NULL) {
		ASSERT ( width >= 0 );
		ASSERT ( height >= 0 );
		//lint -e{737} safe because of asserts!
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
								ASSERT ( p>=0 );
								//lint -e{737} safe because of assert!
								pixelVal += p;
							}
						}
					}
				}
				ASSERT ( pixelCount != 0 );
				ASSERT ( m_sketchBoard != NULL );
				m_sketchBoard[x+y*width] = int(pixelVal/pixelCount);
			}
		}
	}

	CRawImage<bool> *dst=new CRawImage<bool>(width, height);

	//do adaptive thresholding
	for(int x=0; x<width; x++) {
		for(int y=0; y<height; y++) {
			//calculate mean threshold

			ASSERT ( m_sketchBoard != NULL );

			int c = int(src->GetPixel(x,y));
			if(c < m_sketchBoard[x+y*width]-C) {
				dst->SetPixel(x, y, false);
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

int CBinarizer::CalculateOtsuThreshold(const CRawImage<unsigned char> *img) const
{
	ASSERT ( img != NULL );
	//calculate histogram
	int histogram[256];
	for(int i=0; i<256; i++) histogram[i]=0;
	for(int l_pixelIndex=0; 
		l_pixelIndex<img->GetHeight()*img->GetWidth(); 
		l_pixelIndex++) {
		histogram[img->GetPixel(l_pixelIndex)]++;
	}

	int bestThreshold = -1;
	int bestThresholdVariance = 0xfffffff;

	for(int l_threshold=1; l_threshold<255; l_threshold++) {
		int p1 = 0;
		int p2 = 0;
		int mu1 = 0;
		int mu2 = 0;
		int v1 = 0;
		int v2 = 0;

		//calculate p1,p2
		for(int l_lowerIndex=0; 
			l_lowerIndex<=l_threshold; 
			l_lowerIndex++) {
			p1+=histogram[l_lowerIndex];
		}
		for(int l_upperIndex=l_threshold+1; 
			l_upperIndex<256; 
			l_upperIndex++) {
			p2+=histogram[l_upperIndex];
		}

		if(p1 && p2) {

			//calculate mu1, mu2
			for(int l_lower=0; l_lower<=l_threshold; l_lower++) {
				mu1+=histogram[l_lower]*l_lower;
			}
			for(int l_upper=l_threshold+1; l_upper<256; l_upper++) {
				mu2+=histogram[l_upper]*l_upper;
			}
			mu1/=p1;
			mu2/=p2;

			//calculate v1, v2
			{
				for(int l_lower=0; l_lower<=l_threshold; l_lower++) {
					v1+=histogram[l_lower]*(l_lower-mu1)*(l_lower-mu1);
				}
				for(int l_upper=l_threshold+1; l_upper<256; l_upper++) {
					v2+=histogram[l_upper]*(l_upper-mu2)*(l_upper-mu2);
				}
			}
			v1/=p1;
			v2/=p2;

			int variance = p1*v1 + p2*v2;
			if(variance<bestThresholdVariance) {
				bestThreshold = l_threshold;
				bestThresholdVariance=variance;
			}
		}
	}

	//LOG("find otsu: %i\n", bestThreshold);

	return bestThreshold;
}

void CBinarizer::Init(void)
{
	delete[] m_sketchBoard;
	m_sketchBoard = NULL;
}

void CBinarizer::CalcDistanceMap(const CRawImage<bool>* img)
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

	{
		for(int x=1; x<img->GetWidth()-1; x++) {
			for(int y=1; y<img->GetHeight()-1; y++) {
				if(!img->GetPixel(x,y)) {
					int l_westVal = m_distanceMap->GetPixel(x-1,y)+3;
					int l_southVal = m_distanceMap->GetPixel(x,y-1)+3;
					int l_southWestVal = m_distanceMap->GetPixel(x-1,y-1)+4;
					int l_newVal = min(l_southVal, l_westVal );
					l_newVal = min(l_newVal, l_southWestVal);
					m_distanceMap->SetPixel(x, y, l_newVal);
				}
			}
		}
	}
	//pass 2
	{
		for(int x=img->GetWidth()-2; x>0; x--) {
			for(int y=img->GetHeight()-2; y>0; y--) {
				if(!img->GetPixel(x,y)) {
					int l_myVal = m_distanceMap->GetPixel(x,y);
					int l_eastVal = m_distanceMap->GetPixel(x+1,y)+3;
					int l_northVal = m_distanceMap->GetPixel(x,y+1)+3;
					int l_northEastVal = m_distanceMap->GetPixel(x+1,y+1)+4;

					int l_newVal = min( l_myVal, l_eastVal );
					l_newVal = min( l_newVal, l_northVal );
					l_newVal = min( l_newVal, l_northEastVal );
					m_distanceMap->SetPixel(x,y,l_newVal);
				}
			}
		}
	}
}

const CRawImage<int>* CBinarizer::GetDistanceMap() const {
	return m_distanceMap;
}

void CBinarizer::Reset(void)
{
	delete[] m_sketchBoard;
	m_sketchBoard = NULL;

	m_isInitialized = false;
}
