#include "StdAfx.h"
#include ".\binarizer.h"

CBinarizer *CBinarizer::_instance = 0;

CBinarizer::CBinarizer(void)
{
	SetParam("threshold",128);
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

	for(int i=0; i<src->GetWidth()*src->GetHeight(); i++) {
		if(src->GetPixel(i)<threshold) {
			dst->SetPixel(i,0);
		} else {
			dst->SetPixel(i,0xffffff);
		}
	}
	return dst;
}
