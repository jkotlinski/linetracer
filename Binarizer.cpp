#include "StdAfx.h"
#include "binarizer.h"

#include "ProjectSettings.h"
#include ".\binarizer.h"
#include "math.h"
#include <iostream>

namespace ImageProcessing {

CBinarizer::CBinarizer(void)
: CImageProcessor()
, m_sketchBoard(NULL)
, m_isInitialized(false)
{
	LOG("init binarizer\n");
	SetName(CString("Binarizer"));
	SetType ( BINARIZER );
}

CBinarizer::~CBinarizer(void)
{
	delete[] m_sketchBoard;
	m_sketchBoard = NULL;
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
		//std::cout << MIN_THRESHOLD << endl;
		//MIN_THRESHOLD = CalculateKittlerThreshold(src);
		//cout << MIN_THRESHOLD << endl;
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

				for(
					int j = -min(y,WINDOW_SIZE); 
					( j <= WINDOW_SIZE ) && ( y + j < height ); 
					j++ 
					) 
				{
					int l_curr_y = y+j;
					for(
						int i = -min(x,WINDOW_SIZE); 
						(i <= WINDOW_SIZE) && (x+i < width); 
						i++) 
					{
						pixelVal += src->GetPixel(x+i, l_curr_y);
						pixelCount++;
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
		l_pixelIndex++) 
	{
		histogram[img->GetPixel(l_pixelIndex)]++;
	}

	int bestThreshold = -1;
	long long bestThresholdVariance = 0xfffffffffffffff;

	for(int l_threshold=1; l_threshold<255; l_threshold++) 
	{
		long long p1 = 0;
		long long p2 = 0;

		//calculate p1,p2
		for(int l_lowerIndex=0; 
			l_lowerIndex<=l_threshold; 
			l_lowerIndex++) 
		{
			p1+=histogram[l_lowerIndex];
		}
		for(int l_upperIndex=l_threshold+1; 
			l_upperIndex<256; 
			l_upperIndex++) 
		{
			p2+=histogram[l_upperIndex];
		}

		if(p1 && p2) 
		{
			long long mu1 = 0;
			long long mu2 = 0;

			//calculate mu1, mu2
			for(int l_lower=0; l_lower<=l_threshold; l_lower++) {
				mu1+=histogram[l_lower]*l_lower;
			}
			for(int l_upper=l_threshold+1; l_upper<256; l_upper++) {
				mu2+=histogram[l_upper]*l_upper;
			}
			mu1/=p1;
			mu2/=p2;

			long long v1 = 0;
			long long v2 = 0;
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

			long long variance = p1*v1 + p2*v2;
			if(variance < bestThresholdVariance) {
				bestThreshold = l_threshold;
				bestThresholdVariance = variance;
			}
		}
	}

	return bestThreshold;
}

void CBinarizer::Init(void)
{
	delete[] m_sketchBoard;
	m_sketchBoard = NULL;
}

void CBinarizer::Reset(void)
{
	delete[] m_sketchBoard;
	m_sketchBoard = NULL;

	m_isInitialized = false;
}

int CBinarizer::CalculateKittlerThreshold(const CRawImage<unsigned char> * a_img)
{
	const int l_max_intensity = 255;
	const int l_histogram_size = l_max_intensity + 1;
	int l_histogram[l_histogram_size];

	/* 
	other code (from http://hebb.cis.uoguelph.ca/~nharvey/projects/autothresh/minerror.c.html )
	*/

	/* Generate the histogram (y[] array) and find n, the maximum grey value. */
	//n = histogram(y,drawable);
	a_img->calculate_histogram(l_histogram, l_histogram_size);

	/* Compute the partial sum arrays A, B, and C. */
	double A[256];
	double B[256];
	double C[256];

	A[0] = l_histogram[0];
	B[0] = C[0] = 0;
	for (int j = 1; j <= l_max_intensity; j++)
	{
		double temp = l_histogram[j]; A[j] = A[j-1] + temp;
		temp *= j;   B[j] = B[j-1] + temp;
		temp *= j;   C[j] = C[j-1] + temp;
	}
	/* Find the ideal threshold value by the MINERROR algorithm due to Kittler
	* and Illingworth [2].  The notation comes from a treatment in [1]. */
	
	bool converged = false;
	int tries = 0;

	double mean = B[l_max_intensity]/A[l_max_intensity];
	int tnew = (int)floor(mean);
	const int MAX_TRIES = 5;
	int t = 0;
	do
	{
		tries++;
		do
		{
			t = tnew;
			double mu = B[t]/A[t]; 
			double mu_sq = mu*mu;
			double Adiff = A[l_max_intensity]-A[t];
			if (Adiff == 0) break; /* will lead to division by zero */
			double v = (B[l_max_intensity]-B[t])/Adiff; 
			double v_sq = v*v;
			double p = A[t]/A[l_max_intensity];
			double q = Adiff/A[l_max_intensity];
			double sigma_sq = C[t]/A[t] - mu_sq;
			double tau_sq = (C[l_max_intensity]-C[t])/Adiff - v_sq;
			if (sigma_sq == 0 || tau_sq == 0) break; /* ditto */
			double w0 = 1/sigma_sq - 1/tau_sq;
			double w1 = mu/sigma_sq - v/tau_sq;
			double w2 = mu_sq/sigma_sq - v_sq/tau_sq + log10(sigma_sq*q*q/(tau_sq*p*p));
			if (w1*w1-w0*w2 < 0) break; /* no real solutions */
			tnew = (int)floor((w1+sqrt(w1*w1-w0*w2))/w0);
			converged = (tnew == t);
		}
		while (!converged);

		/* There are 2 ways out of the above loop: convergence to a solution, or
		* breaking out due to a division-by-zero or no-real-roots problem.  If
		* the latter is true, try again with a different starting value. */
		if (!converged) 
		{
			tnew = (int)(floor(mean)-7+(rand()&15));
		}
	}
	while (!converged && tries < MAX_TRIES);

	if ( false == converged )
	{
		assert ( !"kittler: couldn't find appropriate threshold value" );
	}
	return t+1;
}

}