#include "StdAfx.h"
#include "DeSaturator.h"

CDeSaturator::CDeSaturator(void)
: CImageProcessor()
{
	LOG("init desaturator\n");
}

CDeSaturator::~CDeSaturator(void)
{
}

CDeSaturator* CDeSaturator::Instance(void)
{
    static CDeSaturator inst;
    return &inst;
}

CSketchImage* CDeSaturator::Process(CSketchImage* i_src)
{
	CRawImage<ARGB> *src=dynamic_cast<CRawImage<ARGB>*>(i_src);
	ASSERT ( src != NULL );
	CRawImage<unsigned char> *dst=new CRawImage<unsigned char>(src->GetWidth(), src->GetHeight());

	//int threshold=(int)GetParam("threshold");

	for(int i=0; i<src->GetPixels(); i++) {
		ARGB c=src->GetPixel(i);
			
		ARGB brightness=(c&0xff0000)>>16;
		brightness+=(c&0xff00)>>8;
		brightness+=c&0xff;

		dst->SetPixel(i,0xff&(brightness/3));
	}
	return dst;
}

//lint -e{715} some params unused
void CDeSaturator::PaintImage(CSketchImage* a_image, CRawImage<ARGB> *a_canvas) const
{
}
