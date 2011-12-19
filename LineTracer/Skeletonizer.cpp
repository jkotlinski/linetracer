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
#include "Skeletonizer.h"

using namespace std;

CSkeletonizer::CSkeletonizer(void)
: CImageProcessor()
, m_knot_image ( 0 )
{
	SetName ( CString ( "Skeletonizer" ) );
	//LOG("init skeletonizer\n");
}

CSkeletonizer::~CSkeletonizer(void)
{
}

CSkeletonizer* CSkeletonizer::Instance() {
    static CSkeletonizer inst;
    return &inst;
}

CSketchImage* CSkeletonizer::Process(CProjectSettings & a_project_settings, CSketchImage *i_src) {
	CRawImage<bool> *src=dynamic_cast<CRawImage<bool>*>(i_src);
	ASSERT ( src != NULL );

	CLineImage li ( i_src->GetWidth(), i_src->GetHeight() );

	//trace lines belonging to knots
	TraceKnotLines(src, li);

	//trace simple lines = lines with two end points
	TraceSimpleLines ( *src, li );

	//maybe we have a bunch of lines left, that have no end points? must be circles.
	TraceCircles ( *src, li );

	LOG("skeletonizer->process complete\n");
	li.UpdateTailData();
	CLineImage* li2 = li.SmoothPositions();

	return li2;
}

void CSkeletonizer::TraceKnotLines(CRawImage< bool >* src, CLineImage & li)
{
	// get a map with all knots and knot neighbors.
	CreateKnotImage(src);

	Vectorize(src, li);

	delete m_knot_image;
	m_knot_image = 0;
}

void CSkeletonizer::TraceSimpleLines ( CRawImage<bool> &segmentMap, CLineImage &li )
{
	for(int x=1; x<segmentMap.GetWidth()-1; x++) {
		for(int y=1; y<segmentMap.GetHeight()-1; y++) {
			if(IsEndPoint(&segmentMap, CPoint(x,y))) {
				CPoint p,p_prev;
				CPolyLine *line = new CPolyLine();
				//add first point
				line->Add(new CSketchPoint(x,y,true));
				segmentMap.SetPixel(x,y,0);
				p_prev = FindSegmentNeighbor(&segmentMap,CPoint(x,y));

				for (;;) {
					p = FindSegmentNeighbor(&segmentMap,p_prev);
					bool l_found_neighbor = ( -1 != p.x );
					if ( l_found_neighbor ) {
						line->Add(new CSketchPoint(p_prev.x, p_prev.y));
						segmentMap.SetPixel(int(p_prev.x),int(p_prev.y),0);
						p_prev = p;
					} else {
						//no neighbor found. add last point
						line->Add(new CSketchPoint(p_prev.x,p_prev.y,true));
						segmentMap.SetPixel(int(p_prev.x),int(p_prev.y),0);
						break;
					}
				}
				li.Add(line);
			}
		}
	}
}

void CSkeletonizer::TraceCircles( CRawImage<bool> &segmentMap, CLineImage &li )
{
	for(int y=1; y<segmentMap.GetHeight()-1; y++) 
	{
		for(int x=1; x<segmentMap.GetWidth()-1; x++) 
		{
			if(segmentMap.GetPixel(x,y)) 
			{
				CPolyLine *l_circleLine = new CPolyLine();

				//found the top pixel of a circle!

				//add first point
				CPoint l_startPoint(x,y);
				segmentMap.SetPixel(x, y, 0);
				l_circleLine->Add(new CSketchPoint(x,y,true));

				CPoint l_previousPoint(x,y);

				while( true ) 
				{
					CPoint p=FindSegmentNeighbor(&segmentMap, l_previousPoint);
					if ( p.x == -1 ) 
					{
						//found end of circle
						l_circleLine->Add(new CSketchPoint(l_startPoint.x, l_startPoint.y, true));
						break;
					}
					l_circleLine->Add(new CSketchPoint(p.x, p.y));
					segmentMap.SetPixel(p.x, p.y, 0);
					l_previousPoint = p;
				}
				ASSERT ( l_circleLine->Size() > 1 );
				li.Add( l_circleLine );
			}
		}
	}
}

/* 
denna funktion detekterar om pixeln vid position (x,y) i bilden image är en 
förgrening.
detta görs genom att räkna hur många separata riktningar det "sticker ut" linjer från 
denna pixel.
om det går ut linjer i fler än två separata riktningar, är detta en knutpunkt,
varvid funktionen returnerar sant. annars falskt.
*/
bool CSkeletonizer::IsKnot(CRawImage<bool>* image, DWORD x, DWORD y)
{
	return GetSeparateNeighborsCount(image, x, y) > 2;
}

int CSkeletonizer::GetSeparateNeighborsCount(CRawImage< bool >* image, Gdiplus::ARGB x, Gdiplus::ARGB y)
{
	if ( !image->GetPixel(x,y) ) return 0;

	int l_direction_matrix[8];
	int l_neighbors = 0;

	/* varje riktning (n, nö, ö, sö, ... har sitt eget id mellan 1-8.)
	id 0 betyder att det inte finns någon förgrundspixel i riktningen. */
	l_direction_matrix[0] = image->GetPixel(x-1,y)   ? 1 : 0; //n
	l_direction_matrix[1] = image->GetPixel(x-1,y-1) ? 2 : 0; //nv
	l_direction_matrix[2] = image->GetPixel(x,y-1)   ? 3 : 0; //v
	l_direction_matrix[3] = image->GetPixel(x+1,y-1) ? 4 : 0; //sv
	l_direction_matrix[4] = image->GetPixel(x+1,y)   ? 5 : 0; //s
	l_direction_matrix[5] = image->GetPixel(x+1,y+1) ? 6 : 0; //sö
	l_direction_matrix[6] = image->GetPixel(x,y+1)   ? 7 : 0; //ö
	l_direction_matrix[7] = image->GetPixel(x-1,y+1) ? 8 : 0; //nö

	// om flera närliggande riktningar är satta, sätter vi dem till samma id.
	if( l_direction_matrix[7] && l_direction_matrix[0] ) {
		l_direction_matrix[0] = l_direction_matrix[7];
	}
	for(int i=0; i<7; i++) {
		if(l_direction_matrix[i+1] && l_direction_matrix[i]) {
			l_direction_matrix[i+1] = l_direction_matrix[i];
		}
	}

	//räkna samman antalet skilda id'n
	bool l_direction_id_slot[8];
	for ( int i=0; i<8; i++ ) {
		l_direction_id_slot[i]=false;
	}
	for ( int i=0; i<8; i++ ) {
		if ( l_direction_matrix[i] ) {
			l_direction_id_slot[l_direction_matrix[i]-1] = true;
		}
	}
	for ( int i=0; i<8; i++ ) {
		if ( l_direction_id_slot[i] ) l_neighbors++;
	}

	return l_neighbors;
}

/*one-pass segmentation algorithm*/
void CSkeletonizer::CreateKnotImage(CRawImage<bool>* image)
{
	map<int,int> labelMap;	
	map<int,int> labelCounter;

	m_knot_image = new CRawImage<bool>(image->GetWidth(), image->GetHeight());
	m_knot_image->Clear();

	for(int y=1; y<image->GetHeight()-1; y++) {	
		for(int x=1; x<image->GetWidth()-1; x++) {
			if ( IsKnot ( image, x, y ) ) {
				m_knot_image->SetPixel ( x,y, true );
			}
		}
	}

	//remove knots from image map
	for(int x=1; x<image->GetWidth()-1; x++) {
		for(int y=1; y<image->GetHeight()-1; y++) {
			if ( m_knot_image->GetPixel(x,y) ) {
				image->SetPixel(x,y,false);
			}
		}
	}
}

void CSkeletonizer::Vectorize(CRawImage<bool>* segmentImg, CLineImage & li)
{
	//trace lines with knot endpoints
	for(int y=1; y<m_knot_image->GetHeight()-1; y++) {
		for ( int x=1; x<m_knot_image->GetWidth()-1; x++ ) {
			if ( m_knot_image->GetPixel(x,y) ) {
				//is a knot

				for (;;) {
					/* ok.. time to start tracing. find any neighbor to this knot */
					CPoint l_startPoint = FindSegmentNeighbor(segmentImg, CPoint(x,y));
					CSketchPoint l_closest_neighbor ( l_startPoint.x, l_startPoint.y );

					bool l_neighbor_was_found = ( l_closest_neighbor.GetX() != -1 );
					if ( !l_neighbor_was_found ) break;

					//create a new line, add this knot
					CPolyLine* line=new CPolyLine();
					line->Add(new CSketchPoint(x,y,true,true));

					TraceLine(
						segmentImg,
						line, //destination line
						l_closest_neighbor.GetCoords()//, //tracing start point
						);

					li.Add(line);
				}
			}
		}
	}

	//connect neighboring knots
	for(int x=1; x<m_knot_image->GetWidth()-1; x++) {
		for(int y=1; y<m_knot_image->GetHeight()-1; y++) {
			if(m_knot_image->GetPixel(x,y) ) {
				CSketchPoint p=FindNeighborKnot(CPoint(x,y));

				//found neighboring knot
				if( p.GetX() != -1 ) {
					CPolyLine *line=new CPolyLine();
					line->Add(new CSketchPoint(x,y,true,true));
					line->Add(p.Clone());
					li.Add(line);

					//clear knot
					m_knot_image->SetPixel(x,y,0);
				}
			}
		}
	}
}

void CSkeletonizer::TraceLine(CRawImage<bool>* segmentImage, CPolyLine* line, CFPoint start)
{
	CSketchPoint p(start);

	CFPoint endKnot=IsKnotNeighbor(p.GetCoords());

	if(endKnot.GetX()!=-1 && NoOrthogonalNeighbors(segmentImage, p.GetCoords())) {
		//point is a knot neighbor
		//add point
		line->Add(p.Clone());

		//clear point
		segmentImage->SetPixel((int)p.GetX(), (int)p.GetY(), 0);

		//add neighbor knot
		line->Add(new CSketchPoint(endKnot,true,true));
		return;
	}

	for(;;) {
		assert(p.GetX() >= -1);
		assert(p.GetY() >= -1);

		//get neigbor point
		CPoint l_newCoords = FindSegmentNeighbor(segmentImage,
			CPoint ( static_cast<int>(p.GetCoords().GetX()), 
				static_cast<int>(p.GetCoords().GetY())) );
		CSketchPoint p_new(l_newCoords.x, l_newCoords.y);

		if(p_new.GetX() == -1) {
			//add point as endpoint
			p.SetIsEndPoint(true);
			line->Add(p.Clone());

			//clear point
			segmentImage->SetPixel((int)p.GetX(), (int)p.GetY(), 0);

			//reached end of line
			break;
		}

		//add point to line
		line->Add(p.Clone());

		assert(p_new.GetX() >= -1);
		assert(p_new.GetY() >= -1);

		//clear old pixel
		segmentImage->SetPixel((int)p.GetX(),(int)p.GetY(),0);

		endKnot = IsKnotNeighbor(p_new.GetCoords());

		if(endKnot.GetX()!=-1 && NoOrthogonalNeighbors(segmentImage, p_new.GetCoords())) {
			//point is a knot neighbor
			//add point
			line->Add(p_new.Clone());

			//clear point
			segmentImage->SetPixel((int)p_new.GetX(),(int)p_new.GetY(),0);

			//add neighbor knot
			line->Add(new CSketchPoint(endKnot,true,true));
			break;
		}
		p=p_new;
	}
}

/* return true if the point is a knot neighbor */
CFPoint CSkeletonizer::IsKnotNeighbor(CFPoint point)
{
	double x=point.GetX();
	double y=point.GetY();

	if(!x || x==m_knot_image->GetWidth()-1) return CFPoint(-1,-1);
	if(!y || y==m_knot_image->GetHeight()-1) return CFPoint(-1,-1);

	int p;

	p=m_knot_image->GetPixel(int(x-1),(int)y);
	if(p) return CFPoint(x-1,y);
	p=m_knot_image->GetPixel(int(x+1),int(y));
	if(p) return CFPoint(x+1,y);
	p=m_knot_image->GetPixel(int(x),int(y-1));
	if(p) return CFPoint(x,y-1);
	p=m_knot_image->GetPixel(int(x),int(y+1));
	if(p) return CFPoint(x,y+1);

	p=m_knot_image->GetPixel(int(x+1),int(y+1));
	if(p) return CFPoint(x+1,y+1);
	p=m_knot_image->GetPixel(int(x+1),int(y-1));
	if(p) return CFPoint(x+1,y-1);
	p=m_knot_image->GetPixel(int(x-1),int(y+1));
	if(p) return CFPoint(x-1,y+1);
	p=m_knot_image->GetPixel(int(x-1),int(y-1));
	if(p) return CFPoint(x-1,y-1);

	return CFPoint(-1,-1);
}

/* return the position of any knot neighboring p */
CSketchPoint CSkeletonizer::FindNeighborKnot( CPoint p)
{
	//check horizontal+vertical neighbors
	if(m_knot_image->GetPixel(p.x-1,p.y)) return CSketchPoint(p.x-1,p.y,true,true);
	if(m_knot_image->GetPixel(p.x,p.y-1)) return CSketchPoint(p.x,p.y-1,true,true);
	if(m_knot_image->GetPixel(p.x+1,p.y)) return CSketchPoint(p.x+1,p.y,true,true);
	if(m_knot_image->GetPixel(p.x,p.y+1)) return CSketchPoint(p.x,p.y+1,true,true);

	//check diagonal neighbors
	if(m_knot_image->GetPixel(p.x-1,p.y-1)) return CSketchPoint(p.x-1,p.y-1,true,true);
	if(m_knot_image->GetPixel(p.x+1,p.y-1)) return CSketchPoint(p.x+1,p.y-1,true,true);
	if(m_knot_image->GetPixel(p.x+1,p.y+1)) return CSketchPoint(p.x+1,p.y+1,true,true);
	if(m_knot_image->GetPixel(p.x-1,p.y+1)) return CSketchPoint(p.x-1,p.y+1,true,true);

	return CSketchPoint(CFPoint(-1,-1));
}

/*
return a neighbor foreground point to p.

return CPoint(-1,-1) if we are at the edge of image, or if no neighbor can be found
*/
CPoint CSkeletonizer::FindSegmentNeighbor(CRawImage<bool>* segmentImage, const CPoint & p)
{
	if((p.x<1) || (p.x>=segmentImage->GetWidth()-1)) return CPoint(-1,-1);
	if((p.y<1) || (p.y>=segmentImage->GetHeight()-1)) return CPoint(-1,-1);

	//check orthogonal neighbors
	if(segmentImage->GetPixel(p.x-1,p.y)) return CPoint(p.x-1,p.y);
	if(segmentImage->GetPixel(p.x,p.y-1)) return CPoint(p.x,p.y-1);
	if(segmentImage->GetPixel(p.x+1,p.y)) return CPoint(p.x+1,p.y);
	if(segmentImage->GetPixel(p.x,p.y+1)) return CPoint(p.x,p.y+1);

	//check diagonal neighbors
	if(segmentImage->GetPixel(int(p.x-1),int(p.y-1))) return CPoint(p.x-1,p.y-1);
	if(segmentImage->GetPixel(int(p.x+1),int(p.y-1))) return CPoint(p.x+1,p.y-1);
	if(segmentImage->GetPixel(int(p.x+1),int(p.y+1))) return CPoint(p.x+1,p.y+1);
	if(segmentImage->GetPixel(int(p.x-1),int(p.y+1))) return CPoint(p.x-1,p.y+1);

	// no neighbor found
	return CPoint(-1,-1);
}

bool CSkeletonizer::NoOrthogonalNeighbors(CRawImage<bool>* segmentImage, CFPoint p)
{
	//check horizontal+vertical neighbors
	if(segmentImage->GetPixel(int(p.GetX()-1),(int)p.GetY())) return false;
	if(segmentImage->GetPixel((int)p.GetX(),int(p.GetY()-1))) return false;
	if(segmentImage->GetPixel(int(p.GetX()+1),(int)p.GetY())) return false;
	if(segmentImage->GetPixel((int)p.GetX(),int(p.GetY()+1))) return false;
	return true;
}

bool CSkeletonizer::IsEndPoint(CRawImage<bool>* image, CPoint p)
{
	return 1 == GetSeparateNeighborsCount(image, p.x, p.y);
}
