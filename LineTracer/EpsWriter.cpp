#include "StdAfx.h"
#include "epswriter.h"

#include "LayerManager.h"

#include <vector>

using namespace std;

CEpsWriter::CEpsWriter(void)
{
	LOG("init epswriter\n");
}

CEpsWriter::~CEpsWriter(void)
{
}

CEpsWriter* CEpsWriter::Instance() {
    static CEpsWriter inst;
    return &inst;
}

void CEpsWriter::Write(const CString &FileName)
{
	CStdioFile out(FileName,CFile::modeCreate|CFile::typeText|CFile::modeWrite);
	CString str;

	CLayerManager *lm = CLayerManager::Instance();

	CLineImage *lineImage = dynamic_cast<CLineImage*> (lm->GetLastLayer()->GetSketchImage());
	ASSERT ( lineImage != NULL );
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

	for(unsigned int l_lineIndex=0; l_lineIndex<lineImage->Size(); l_lineIndex++) {
		CPolyLine* pl=lineImage->At(l_lineIndex);

		str.Format("%f %f moveto\n",pl->At(0)->GetX(),height-pl->At(0)->GetY());
		out.WriteString(str);

		//-------------------------------------

		for(unsigned int i=0; i<pl->Size()-1; i++) {

			CFPoint p = pl->At(i)->GetControlPointForward();
			str.Format("%f %f ",p.GetX(),height-p.GetY());
			out.WriteString(str);

			p = pl->At(i+1)->GetControlPointBack();
			str.Format("%f %f ",p.GetX(),height-p.GetY());
			out.WriteString(str);

			//endpoint
			p = CFPoint( pl->At(i+1)->GetX(), pl->At(i+1)->GetY() );
			str.Format("%f %f curveto\n",p.GetX(),height-p.GetY());
			out.WriteString(str);

		}

		out.WriteString("stroke\n");
	}
	out.Close();
}
