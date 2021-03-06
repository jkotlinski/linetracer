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
#include "kneesplitter.h"

#include <math.h>

#include "ProjectSettings.h"
#include "LineImage.h"
#include "LayerManager.h"
#include "SketchPoint.h"

CKneeSplitter::CKneeSplitter(void)
: CImageProcessor()
{
	SetName ( CString ( "Knee Splitter" ) );
}

CKneeSplitter::~CKneeSplitter(void)
{
}

CKneeSplitter* CKneeSplitter::Instance() {
	static CKneeSplitter inst;
	return &inst;
}

CSketchImage * CKneeSplitter::Process(CProjectSettings & a_project_settings, CSketchImage* i_src) {
	CLineImage *src = dynamic_cast<CLineImage*>(i_src);
	ASSERT ( src != NULL );

	// LOG ( "KneeSplitter()\n" );

	const double l_threshold = a_project_settings.GetParam(
		CProjectSettings::KNEESPLITTER_THRESHOLD);

	for(unsigned int i=0; i<src->Size(); i++) 
	{
		CPolyLine *line = src->GetLine(i);

		vector<CSketchPoint*>::iterator iter;
		vector<CSketchPoint*>::iterator end;
		iter = line->Begin();
		end = line->End();
		--end;

		CSketchPoint *prev_point = *iter;
		++iter;
		CSketchPoint *point = *iter;

		while(iter!=end) {
			++iter;
			CSketchPoint *next_point = *iter;

			double diffx1 = prev_point->GetX() - point->GetX();
			double diffy1 = prev_point->GetY() - point->GetY();
			double diffx2 = next_point->GetX() - point->GetX();
			double diffy2 = next_point->GetY() - point->GetY();
			double dist1 = sqrt(diffx1*diffx1+diffy1*diffy1);
			double dist2 = sqrt(diffx2*diffx2+diffy2*diffy2);

			diffx1 /= dist1;
			diffy1 /= dist1;
			diffx2 /= dist2;
			diffy2 /= dist2;

			double diff = (diffx1*diffx2+diffy1*diffy2)/2;

			if(diff > l_threshold) {
				point->SetKnee(true);
				//LOG("SETKNEE\n");
			} else {
				point->SetKnee(false);
			}

			//advance to next point
			prev_point = point;
			point = next_point;
		}
	}

	CLineImage *dst = new CLineImage(src->GetWidth(), src->GetHeight());

	for(unsigned int l_lineIndex=0; l_lineIndex<src->Size(); ++l_lineIndex) 
	{
		CPolyLine* line = src->GetLine(l_lineIndex);
		CPolyLine* addLine = new CPolyLine();

		vector<CSketchPoint*>::iterator iter = line->Begin();

		while(iter!=line->End()) {
			addLine->Add((*iter)->Clone());
			if((*iter)->IsKnee()) {
				//break line
				//LOG("ISKNEE\n");
				dst->Add(addLine);
				addLine = new CPolyLine();
				addLine->Add((*iter)->Clone());
			}
			++iter;
		}
		dst->Add(addLine);
	}

	return dst;
}
