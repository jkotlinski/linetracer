#include "StdAfx.h"
#include ".\epswriter.h"

#include "LayerManager.h"

CEpsWriter::CEpsWriter(void)
{
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

	for(int i=0; i<lineImage->Size(); i++) {
		CPolyLine* pl=lineImage->At(i);

		if(1/*pl->Size()>3*/) {

			CPoint curr(pl->At(0).x,pl->At(0).y);
			CPoint prev;

			str.Format("%i %i moveto\n",curr.x,height-curr.y);
			out.WriteString(str);

			for(int j=1; j<pl->Size(); j++) {
				prev=curr;
				curr=pl->At(j);

				str.Format("%i %i rlineto ",curr.x-prev.x, prev.y-curr.y);
				out.WriteString(str);
			}
			out.WriteString("stroke\n");
		}
	}
	out.Close();
}
