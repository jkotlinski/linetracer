#include "StdAfx.h"
#include "lineimage.h"

#include "PolyLine.h"

#include <vector>
#include <map>

CLineImage::CLineImage(int width, int height)
: CSketchImage()
, m_polyLines()
{
	//LOG("init lineimage\n");
	SetSize(width,height);
}

CLineImage::CLineImage(void) 
: CSketchImage()
, m_polyLines()
{
	SetSize(0,0);
}

CLineImage::~CLineImage(void)
{
	try {
		//LOG("delete lineimage with size %i\n",m_polyLines.size());
		for(unsigned int i=0; i<m_polyLines.size(); i++) {
			delete m_polyLines.at(i);
		}
	} catch (...) {
		try {
			ASSERT ( false );
		} catch (...) {
		}
	}
}

void CLineImage::Add(CPolyLine* polyLine)
{
	m_polyLines.push_back(polyLine);
}

const unsigned int CLineImage::Size(void) const
{
	return static_cast<unsigned int> ( m_polyLines.size() );
}

CPolyLine* CLineImage::At(unsigned int i) const
{
	return m_polyLines.at( i );
}


void CLineImage::Clear(void)
{
	for(unsigned int i=0; i<m_polyLines.size(); i++) {
		delete m_polyLines.at(i);
	}
	m_polyLines.clear();
}

void CLineImage::SolderKnots(void)
{
	CLogger::Inactivate();

	CLineImage *tmp = new CLineImage(-1,-1);

	map<unsigned int,int> knotCount;
	map<unsigned int,int>::iterator iter;
	map<unsigned int,bool> lineProcessed;

	for(unsigned int line=0; line<Size(); line++) {
		CSketchPoint *p = At(line)->GetHeadPoint();
		unsigned int pp = (int(p->GetX())&0xffff) | (int(p->GetY())<<16);
		knotCount[pp]=knotCount[pp]+1;

		p = At(line)->GetTailPoint();
		pp = (int(p->GetX())&0xffff) | (int(p->GetY())<<16);
		knotCount[pp]=knotCount[pp]+1;

		ASSERT(At(line)->GetHeadPoint()->Distance(At(line)->GetTailPoint()->GetCoords()) 
			> 0.1);
	}

	for( iter=knotCount.begin(); iter!=knotCount.end(); ++iter ) {
		if((*iter).second==2) {
			CFPoint p( double ( iter->first & 0xffff ), 
				double ( iter->first >> 16 ) );

			CPolyLine *l1=0;
			CPolyLine *l2=0;

			for(unsigned int i=0; i<Size(); i++) 
			{
				static const double maxDist = 2;

				if( lineProcessed[i] ) continue;

				CPolyLine *line = At(i);
				if(line->GetHeadPoint()->Distance(p)<maxDist || 
					line->GetTailPoint()->Distance(p)<maxDist) 
				{
					if(l1==0) {
						l1=line;
					} else {
						l2=line;
					}
					lineProcessed[i]=true;
				}
			}

			if(l2==0) {
				LOG( "! circle\n" );
				
				//circle!
				if(l1!=0) 
				{
					Add(l1->Clone());
				}
				else
				{
					LOG ( "don't add ???\n" );
				}
			} else {
				//merge two lines!
				ASSERT ( l1 != NULL );
				CPolyLine *l_newLine = l1->MergeLine(l2);
				Add(l_newLine);
			}
		}
	}

	for(unsigned int i=0; i<Size(); i++) {
		if(!lineProcessed[i]) {
			tmp->Add(At(i)->Clone());
		}
	}

	Clear();

	for(unsigned int l_lineIndex=0; l_lineIndex<tmp->Size(); l_lineIndex++) {
		Add(tmp->At(l_lineIndex)->Clone());
	}

	delete tmp;
}

const int CLineImage::IsKnotInLines(const CFPoint &p) const
{
	int counter=0;

	for(unsigned int i=0; i<Size(); i++) {
		CPolyLine *line = At(i);

		CSketchPoint *linePoint = line->GetHeadPoint();

		if(linePoint->Distance(p)<0.8) {
			counter++;
		} else {
			linePoint = line->GetTailPoint();
			if(linePoint->Distance(p)<0.8) {
				counter++;
			}
		}
	}

	return counter;
}



CLineImage* CLineImage::Clone(void) const
{
	CLineImage *clone = new CLineImage(GetWidth(),GetHeight());

	for(unsigned int i=0; i<Size(); i++) {
		clone->Add(At(i)->Clone());
	}

	return clone;
}

bool CLineImage::IsTail(CPolyLine* pl) const
{
	CLogger::Inactivate();

	LOG("IsTail: ");
	if(IsKnotInLines( pl->GetHeadPoint()->GetCoords() )==1) {
		LOG("true\n");
		return true;
	}
	if(IsKnotInLines( pl->GetTailPoint()->GetCoords() )==1) {
		LOG("true\n");
		return true;
	}
	LOG("false\n");
	return false;
}

CLineImage* CLineImage::SmoothPositions() const
{
	CLineImage *tmp = new CLineImage(GetWidth(),GetHeight());

	for(unsigned int line=0; line<Size(); line++) {
		CPolyLine* pl = At(line);
		CPolyLine* nuLine = pl->SmoothPositions();
		tmp->Add(nuLine);
	}
	return tmp;
}

// check all lines in image and update their tail status
void CLineImage::UpdateTailData(void)
{
	for(unsigned int i=0; i<m_polyLines.size(); i++) {
		CPolyLine *pl = m_polyLines.at(i);
		pl->SetTail( IsTail( pl ) );
	}
}
