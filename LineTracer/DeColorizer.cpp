#include "StdAfx.h"
#include ".\decolorizer.h"

CDeColorizer::CDeColorizer(void)
{
}

CDeColorizer::~CDeColorizer(void)
{
}

CDeColorizer* CDeColorizer::Instance(void)
{
    static CDeColorizer inst;
    return &inst;
}

CSketchImage* CDeColorizer::Process(CSketchImage* i_src)
{
	CRawImage *src=static_cast<CRawImage*>(i_src);
	CRawImage *dst=new CRawImage(src->GetWidth(), src->GetHeight());

	int threshold=(int)GetParam("threshold");

	for(int i=0; i<src->GetPixels(); i++) {
		ARGB c=src->GetPixel(i);
			
		int brightness=(c&0xff0000)>>16;
		brightness+=(c&0xff00)>>8;
		brightness+=c&0xff;

		dst->SetPixel(i,0xff&(brightness/3));
	}
	return dst;
}
