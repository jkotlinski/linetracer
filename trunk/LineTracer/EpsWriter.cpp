#include "StdAfx.h"
#include ".\epswriter.h"

#include "LayerManager.h"

#include <list>
#include <vector>

using namespace std;

#include <math.h>

CEpsWriter::CEpsWriter(void)
{
	TRACE("init epswriter\n");
}

CEpsWriter::~CEpsWriter(void)
{
}

CEpsWriter* CEpsWriter::Instance() {
    static CEpsWriter inst;
    return &inst;
}

void CEpsWriter::Write(CString *FileName)
{
	CStdioFile out(*FileName,CFile::modeCreate|CFile::typeText|CFile::modeWrite);
	CString str;

	CLayerManager *lm = CLayerManager::Instance();

	CLineImage *lineImage = static_cast<CLineImage*>(lm->GetLayer(lm->Layers()-1)->GetSketchImage());

	int width=lineImage->GetWidth();
	int height=lineImage->GetHeight();

	out.WriteString("%%!PS-Adobe-3.0 EPSF-3.0\n");
	out.WriteString("%%BoundingBox: ");
	str.Format("%i %i %i %i\n",0,0,width,height);
	out.WriteString(str);
	//use round pen
	out.WriteString("1 setlinecap\n");
	out.WriteString("1 setlinejoin\n");

	out.WriteString("0.5 setlinewidth\n");

	for(unsigned int i=0; i<lineImage->Size(); i++) {
		CPolyLine* pl=lineImage->At(i);

		str.Format("%f %f moveto\n",pl->At(0)->x,height-pl->At(0)->y);
		out.WriteString(str);

		//-------------------------------------

		for(unsigned int i=0; i<pl->Size()-1; i++) {

			CFPoint p = pl->At(i)->GetControlPointForward();
			str.Format("%f %f ",p.x,height-p.y);
			out.WriteString(str);

			p = pl->At(i+1)->GetControlPointBack();
			str.Format("%f %f ",p.x,height-p.y);
			out.WriteString(str);

			//endpoint
			p.x = pl->At(i+1)->x;
			p.y = pl->At(i+1)->y;
			str.Format("%f %f curveto\n",p.x,height-p.y);
			out.WriteString(str);

		}

		out.WriteString("stroke\n");
	}
	out.Close();
}
