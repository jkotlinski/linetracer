#include "StdAfx.h"
#include ".\binarizer.h"

CBinarizer *CBinarizer::_instance = 0;

CBinarizer::CBinarizer(void)
{
	SetParam("threshold",128);
}

CBinarizer::~CBinarizer(void)
{
}

CBinarizer *CBinarizer::Instance() {
	if(_instance == 0) {
		_instance = new CBinarizer();
	}
	return _instance;
}

CRawImage* CBinarizer::Process(CRawImage* src)
{
	TRACE("start processing\n");
	CRawImage *dst=new CRawImage(src->GetWidth(), src->GetHeight());

	int threshold=(int)GetParam("threshold")*3;

	for(int x=0; x<src->GetWidth(); x++) {
		TRACE("x: %i\n",x);
		for(int y=0; y<src->GetHeight(); y++) {
			ARGB c=src->GetPixel(x,y);
			
			int brightness=(c&0xff0000)>>16;
			brightness+=(c&0xff00)>>8;
			brightness+=c&0xff;

			if(brightness<threshold) {
				dst->SetPixel(x,y,0);
			} else {
				dst->SetPixel(x,y,0xffffff);
			}
		}
	}
	TRACE("stop processing\n");
	return dst;
}
