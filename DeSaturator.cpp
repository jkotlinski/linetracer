#include "StdAfx.h"
#include "DeSaturator.h"

CDeSaturator::CDeSaturator(void)
: CImageProcessor()
{
	//LOG("init desaturator\n");
	SetName(CString("desaturator"));
}

CDeSaturator::~CDeSaturator(void)
{
}

CDeSaturator* CDeSaturator::Instance(void)
{
    static CDeSaturator inst;
    return &inst;
}

CSketchImage* CDeSaturator::Process(CProjectSettings & a_project_settings, CSketchImage* i_src)
{
	CRawImage<ARGB> *src=dynamic_cast<CRawImage<ARGB>*>(i_src);
	ASSERT ( src != NULL );
	CRawImage<unsigned char> *dst=new CRawImage<unsigned char>(src->GetWidth(), src->GetHeight());

	//int threshold=(int)GetParam("threshold");

	for(int i=0; i<src->GetPixelCount(); i++) {
		ARGB c=src->GetPixel(i);
			
		ARGB brightness=(c&0xff0000)>>16;
		brightness+=(c&0xff00)>>8;
		brightness+=c&0xff;

		dst->SetPixel(i, static_cast<unsigned char>(brightness/3));
	}
	return dst;
}
