#include "StdAfx.h"
#include ".\binarizer.h"

CBinarizer *CBinarizer::_instance = 0;

CBinarizer::CBinarizer(void)
{
	SetParam("threshold",-1);
}

CBinarizer::~CBinarizer(void)
{
	if(_instance!=0) delete _instance;
}

CBinarizer *CBinarizer::Instance() {
	if(_instance == 0) {
		_instance = new CBinarizer();
	}
	return _instance;
}

CRawImage* CBinarizer::Process(CRawImage* src)
{
	CRawImage *dst=new CRawImage(src->GetWidth(), src->GetHeight());

	ARGB threshold=(ARGB)GetParam("threshold");

	if(threshold == -1) {
		threshold = CalculateOtsuThreshold(src);
		SetParam("threshold", threshold);
	}

	for(int i=0; i<src->GetWidth()*src->GetHeight(); i++) {
		if(src->GetPixel(i)<threshold) {
			dst->SetPixel(i,0);
		} else {
			dst->SetPixel(i,0xffffff);
		}
	}
	return dst;
}

int CBinarizer::CalculateOtsuThreshold(CRawImage *img)
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

	return bestThreshold;
}
