#include "StdAfx.h"
#include ".\decolorizer.h"

CDeColorizer* CDeColorizer::_instance = 0;

CDeColorizer::CDeColorizer(void)
{
}

CDeColorizer::~CDeColorizer(void)
{
	if(_instance!=0) delete _instance;
}

CDeColorizer* CDeColorizer::Instance(void)
{
	if(_instance == 0) {
		_instance = new CDeColorizer();
	}
	return _instance;
}

CRawImage* CDeColorizer::Process(CRawImage* src)
{
	CRawImage *dst=new CRawImage(src->GetWidth(), src->GetHeight());

	int threshold=(int)GetParam("threshold");

	for(int i=0; i<src->GetWidth()*src->GetHeight(); i++) {
		ARGB c=src->GetPixel(i);
			
		int brightness=(c&0xff0000)>>16;
		brightness+=(c&0xff00)>>8;
		brightness+=c&0xff;

		dst->SetPixel(i,0xff&(brightness/3));
	}
	return dst;
}
