/** This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

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
