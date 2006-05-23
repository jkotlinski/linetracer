#include "StdAfx.h"
#include "Skeletonizer.h"

#include "EpsWriter.h"
#include "ProjectSettings.h"

#include <math.h>
#include <map>
#include <vector>
#include <map>
#include <assert.h>

using namespace std;

const unsigned int k_knot_mask = 0x80000000;
const unsigned int k_knot_neighbor_mask = 0x40000000;

//#define DEFINE_BLOCK_ALLOCATED_MAP
//#define DEFINE_BLOCK_ALLOCATED_MULTIMAP
//#include "blockallocator.h"

CSkeletonizer::CSkeletonizer(void)
: CImageProcessor()
{
	SetName ( CString ( "Skeletonizer" ) );
	LOG("init skeletonizer\n");
}

CSkeletonizer::~CSkeletonizer(void)
{
}

CSkeletonizer* CSkeletonizer::Instance() {
    static CSkeletonizer inst;
    return &inst;
}

CSketchImage* CSkeletonizer::Process(CSketchImage *i_src) {
	CRawImage<bool> *src=dynamic_cast<CRawImage<bool>*>(i_src);
	ASSERT ( src != NULL );
	/*CRawImage<ARGB> *distMap=new CRawImage<ARGB>(src->GetWidth(), src->GetHeight());
	distMap->Clear();
	DistanceTransform(src,distMap,3,4);*/
	
	CRawImage<ARGB>* knotImage=new CRawImage<ARGB>(src->GetWidth(),src->GetHeight());
	knotImage->Clear();

	/* get a map with all line pixels marked with a line of the respective id.
	also the knot image is filled with the positions of all knots.
	*/
	(void) CreateKnotImage(src,knotImage);

	CLineImage* li = Vectorize(src, knotImage);

	delete knotImage;

	//trace simple lines = lines with two end points
	TraceSimpleLines ( *src, *li );

	//maybe we have a bunch of lines left, that have no end points? must be circles.
	TraceCircles ( *src, *li );

	LOG("skeletonizer->process complete\n");
	li->UpdateTailData();
	CLineImage* li2 = li->SmoothPositions();
	delete li;
	return li2;
}

void CSkeletonizer::TraceSimpleLines ( CRawImage<bool> &segmentMap, CLineImage &li )
{
	for(int x=1; x<segmentMap.GetWidth()-1; x++) {
		for(int y=1; y<segmentMap.GetHeight()-1; y++) {
			if(segmentMap.GetPixel(x,y) && IsEndPoint(&segmentMap, CPoint(x,y))) {
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

void CSkeletonizer::DistanceTransform(CRawImage<bool> *src, CRawImage<ARGB> *dst, int DirectDistance, int IndirectDistance)
{
	//clean image
	{
		for(int x=0; x<src->GetWidth()*src->GetHeight(); x++) {
			dst->SetPixel(x,0);
		}
	}

	for(int y=1; y<src->GetHeight()-1; y++) {
		for(int x=1; x<src->GetWidth()-1; x++) {
			if(!src->GetPixel(x,y)) {
				ARGB dist=dst->GetPixel(x-1,y-1)+IndirectDistance;
				dist=min(dist,dst->GetPixel(x,y-1)+DirectDistance);
				dist=min(dist,dst->GetPixel(x+1,y-1)+IndirectDistance);
				dist=min(dist,dst->GetPixel(x-1,y)+DirectDistance);
				dst->SetPixel(x,y,dist);
			}
		}
	}

	for(int y=src->GetHeight()-2; y>0; y--) {
		for(int x=src->GetWidth()-2; x>0; x--) {
			if(!src->GetPixel(x,y)) {
				ARGB dist=dst->GetPixel(x+1,y+1)+IndirectDistance;
				dist=min(dist,dst->GetPixel(x,y+1)+DirectDistance);
				dist=min(dist,dst->GetPixel(x-1,y+1)+IndirectDistance);
				dist=min(dist,dst->GetPixel(x+1,y)+DirectDistance);
				if(dist<dst->GetPixel(x,y)) {
					dst->SetPixel(x,y,dist);
				}
			}
		}
	}
}

CRawImage<ARGB>* CSkeletonizer::DeleteNonMaximumsSimple(CRawImage<ARGB>* dst)
{
	CRawImage<ARGB> *tmp=new CRawImage<ARGB>(dst->GetWidth(), dst->GetHeight());

	for(int y=1; y<dst->GetHeight()-1; y++) {
		for(int x=1; x<dst->GetWidth()-1; x++) {
			bool isLocalMaximum=false;

			ARGB val=dst->GetPixel(x,y);

			//check absolute local maximum
			if(val>dst->GetPixel(x-1,y) && val>=dst->GetPixel(x+1,y)) 
			{
				isLocalMaximum=true;
			} else if(val>dst->GetPixel(x,y-1) && val>=dst->GetPixel(x,y+1))
			{
				isLocalMaximum=true;
			}

			tmp->SetPixel(x,y,isLocalMaximum?0xff:0);
		}
	}

	return tmp;
}

/* 
denna funktion detekterar om pixeln vid position (x,y) i bilden image �r en 
f�rgrening.
detta g�rs genom att r�kna hur m�nga separata riktningar det "sticker ut" linjer fr�n 
denna pixel.
om det g�r ut linjer i fler �n tv� separata riktningar, �r detta en knutpunkt,
och ett unikt id f�r denna knutpunkt returneras. annars returneras 0.
*/
int CSkeletonizer::IsKnot(CRawImage<bool>* image, DWORD x, DWORD y)
{
	static int currentKnot = 1;

	int l_direction_matrix[8];
	int l_neighbors = 0;

	/* varje riktning (n, n�, �, s�, ... har sitt eget id mellan 1-8.)
	id 0 betyder att det inte finns n�gon f�rgrundspixel i riktningen. */
	l_direction_matrix[0] = image->GetPixel(x-1,y)   ? 1 : 0; //n
	l_direction_matrix[1] = image->GetPixel(x-1,y-1) ? 2 : 0; //nv
	l_direction_matrix[2] = image->GetPixel(x,y-1)   ? 3 : 0; //v
	l_direction_matrix[3] = image->GetPixel(x+1,y-1) ? 4 : 0; //sv
	l_direction_matrix[4] = image->GetPixel(x+1,y)   ? 5 : 0; //s
	l_direction_matrix[5] = image->GetPixel(x+1,y+1) ? 6 : 0; //s�
	l_direction_matrix[6] = image->GetPixel(x,y+1)   ? 7 : 0; //�
	l_direction_matrix[7] = image->GetPixel(x-1,y+1) ? 8 : 0; //n�

	// om flera n�rliggande riktningar �r satta, s�tter vi dem till samma id.
	if( l_direction_matrix[7] && l_direction_matrix[0] ) {
		l_direction_matrix[0] = l_direction_matrix[7];
	}
	for(int i=0; i<7; i++) {
		if(l_direction_matrix[i+1] && l_direction_matrix[i]) {
			l_direction_matrix[i+1] = l_direction_matrix[i];
		}
	}

	//r�kna samman antalet skilda id'n
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

	return ( l_neighbors > 2 ) ? currentKnot++ : 0;
}


/*one-pass segmentation algorithm*/
void CSkeletonizer::CreateKnotImage(CRawImage<bool>* image, CRawImage<ARGB>* knotImage)
{
	map<int,int> labelMap;	
	map<int,int> labelCounter;	

	//paint knots red
	for(int x=1; x<image->GetWidth()-1; x++) {
		for(int y=1; y<image->GetHeight()-1; y++) {
	
			if(image->GetPixel(x,y)) {
				int l_knot_id = IsKnot(image,x,y);
				if(l_knot_id) {
					//mark knot
					knotImage->SetPixel(x,y, k_knot_mask | l_knot_id);

					//remove knot from image map
					//image->SetPixel(x,y,0);

					//mark knot neighbors
					if(!(knotImage->GetPixel(x-1,y) & k_knot_mask)) {
						knotImage->SetPixel(x-1,y, k_knot_neighbor_mask|l_knot_id);
					}
					if(!(knotImage->GetPixel(x+1,y)&k_knot_mask)) {
						knotImage->SetPixel(x+1,y, k_knot_neighbor_mask|l_knot_id);
					}
					if(!(knotImage->GetPixel(x,y+1)&k_knot_mask)) {
						knotImage->SetPixel(x,y+1, k_knot_neighbor_mask|l_knot_id);
					}
					if(!(knotImage->GetPixel(x,y-1)&k_knot_mask)) {
						knotImage->SetPixel(x,y-1, k_knot_neighbor_mask|l_knot_id);
					}
					if(!(knotImage->GetPixel(x-1,y-1)&k_knot_mask)) {
						knotImage->SetPixel(x-1,y-1, k_knot_neighbor_mask|l_knot_id);
					}
					if(!(knotImage->GetPixel(x-1,y+1)&k_knot_mask)) {
						knotImage->SetPixel(x-1,y+1,  k_knot_neighbor_mask|l_knot_id);
					}
					if(!(knotImage->GetPixel(x+1,y-1)&k_knot_mask)) {
						knotImage->SetPixel(x+1,y-1,  k_knot_neighbor_mask|l_knot_id);
					}
					if(!(knotImage->GetPixel(x+1,y+1)&k_knot_mask)) {
						knotImage->SetPixel(x+1,y+1,  k_knot_neighbor_mask|l_knot_id);
					}
				}
			}
		}
	}

	//remove knots from image map
	for(int x=1; x<image->GetWidth()-1; x++) {
		for(int y=1; y<image->GetHeight()-1; y++) {
			if ( knotImage->GetPixel(x,y) & k_knot_mask ) {
				image->SetPixel(x,y,false);
			}
		}
	}
}

CLineImage* CSkeletonizer::Vectorize(CRawImage<bool>* segmentImg, CRawImage<ARGB>* knotImg)
{
	CLineImage* li=new CLineImage(knotImg->GetWidth(), knotImg->GetHeight());

	//trace lines with knot endpoints
	for(int y=1; y<knotImg->GetHeight()-1; y++) {
		for ( int x=1; x<knotImg->GetWidth()-1; x++ ) {
			if ( knotImg->GetPixel(x,y) & k_knot_mask ) {
				//is a knot

				/* forbid this knot as an end point for the current trace.
				why do we want to forbid this knot as an end point?
				because a line can't end where it starts! */
				map<int,bool> forbiddenEndKnotIds;
				forbiddenEndKnotIds[knotImg->GetPixel(x,y)&0xffffff]=true;

				/* also forbid any neighboring knots as end points for the current trace.*/
				int p = knotImg->GetPixel(x+1,y);
				if(p&k_knot_mask) forbiddenEndKnotIds[p&0xffffff]=true;
				p=knotImg->GetPixel(x-1,y);
				if(p&k_knot_mask) forbiddenEndKnotIds[p&0xffffff]=true;
				p=knotImg->GetPixel(x,y+1);
				if(p&k_knot_mask) forbiddenEndKnotIds[p&0xffffff]=true;
				p=knotImg->GetPixel(x,y-1);
				if(p&k_knot_mask) forbiddenEndKnotIds[p&0xffffff]=true;
				p=knotImg->GetPixel(x-1,y-1);
				if(p&k_knot_mask) forbiddenEndKnotIds[p&0xffffff]=true;
				p=knotImg->GetPixel(x-1,y+1);
				if(p&k_knot_mask) forbiddenEndKnotIds[p&0xffffff]=true;
				p=knotImg->GetPixel(x+1,y-1);
				if(p&k_knot_mask) forbiddenEndKnotIds[p&0xffffff]=true;
				p=knotImg->GetPixel(x+1,y+1);
				if(p&k_knot_mask) forbiddenEndKnotIds[p&0xffffff]=true;

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
						knotImg,
						line, //destination line
						l_closest_neighbor.GetCoords(), //tracing start point
						&forbiddenEndKnotIds
						);

					li->Add(line);
				}
			}
		}
	}

	//connect neighboring knots
	for(int x=1; x<knotImg->GetWidth()-1; x++) {
		for(int y=1; y<knotImg->GetHeight()-1; y++) {
			if(knotImg->GetPixel(x,y)&k_knot_mask) {
				CSketchPoint p=FindNeighborKnot(knotImg, CPoint(x,y));

				//found neighboring knot
				if( p.GetX() != -1 ) {
					CPolyLine *line=new CPolyLine();
					line->Add(new CSketchPoint(x,y,true,true));
					line->Add(p.Clone());
					li->Add(line);

					//clear knot
					knotImg->SetPixel(x,y,0);
				}
			}
		}
	}

	return li;
}

void CSkeletonizer::TraceLine(CRawImage<bool>* segmentImage, CRawImage<ARGB>* knotImage, CPolyLine* line, CFPoint start, map<int,bool> *forbiddenEndKnotIds)
{
	CSketchPoint p(start);

	CFPoint endKnot=IsKnotNeighbor(knotImage, p.GetCoords(), forbiddenEndKnotIds);

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

	while(1) {
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

		endKnot = IsKnotNeighbor(knotImage, p_new.GetCoords(), forbiddenEndKnotIds);

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
CFPoint CSkeletonizer::IsKnotNeighbor(CRawImage<ARGB>* knotImage, CFPoint point, map<int,bool> *forbiddenEndKnotIds)
{
	double x=point.GetX();
	double y=point.GetY();

	if(!x || x==knotImage->GetWidth()-1) return CFPoint(-1,-1);
	if(!y || y==knotImage->GetHeight()-1) return CFPoint(-1,-1);

	if(!(knotImage->GetPixel((int)x,(int)y)&k_knot_neighbor_mask)) return CFPoint(-1,-1);

	int p;

	p=knotImage->GetPixel(int(x-1),(int)y);
	if((p&k_knot_mask) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CFPoint(x-1,y);
	p=knotImage->GetPixel(int(x+1),int(y));
	if((p&k_knot_mask) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CFPoint(x+1,y);
	p=knotImage->GetPixel(int(x),int(y-1));
	if((p&k_knot_mask) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CFPoint(x,y-1);
	p=knotImage->GetPixel(int(x),int(y+1));
	if((p&k_knot_mask) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CFPoint(x,y+1);

	p=knotImage->GetPixel(int(x+1),int(y+1));
	if((p&k_knot_mask) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CFPoint(x+1,y+1);
	p=knotImage->GetPixel(int(x+1),int(y-1));
	if((p&k_knot_mask) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CFPoint(x+1,y-1);
	p=knotImage->GetPixel(int(x-1),int(y+1));
	if((p&k_knot_mask) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CFPoint(x-1,y+1);
	p=knotImage->GetPixel(int(x-1),int(y-1));
	if((p&k_knot_mask) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CFPoint(x-1,y-1);

	return CFPoint(-1,-1);
}

/* return the position of any knot neighboring p */
CSketchPoint CSkeletonizer::FindNeighborKnot(CRawImage<ARGB>* knotImg, CPoint p)
{
	//check horizontal+vertical neighbors
	if(knotImg->GetPixel(p.x-1,p.y)&k_knot_mask) return CSketchPoint(p.x-1,p.y,true,true);
	if(knotImg->GetPixel(p.x,p.y-1)&k_knot_mask) return CSketchPoint(p.x,p.y-1,true,true);
	if(knotImg->GetPixel(p.x+1,p.y)&k_knot_mask) return CSketchPoint(p.x+1,p.y,true,true);
	if(knotImg->GetPixel(p.x,p.y+1)&k_knot_mask) return CSketchPoint(p.x,p.y+1,true,true);

	//check diagonal neighbors
	if(knotImg->GetPixel(p.x-1,p.y-1)&k_knot_mask) return CSketchPoint(p.x-1,p.y-1,true,true);
	if(knotImg->GetPixel(p.x+1,p.y-1)&k_knot_mask) return CSketchPoint(p.x+1,p.y-1,true,true);
	if(knotImg->GetPixel(p.x+1,p.y+1)&k_knot_mask) return CSketchPoint(p.x+1,p.y+1,true,true);
	if(knotImg->GetPixel(p.x-1,p.y+1)&k_knot_mask) return CSketchPoint(p.x-1,p.y+1,true,true);

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
	int x=p.x;
	int y=p.y;
	int matrix[8];

	int neighbors=0;

	matrix[0]=image->GetPixel(x-1,y)?1:0;
	matrix[1]=image->GetPixel(x-1,y-1)?2:0;
	matrix[2]=image->GetPixel(x,y-1)?3:0;
	matrix[3]=image->GetPixel(x+1,y-1)?4:0;
	matrix[4]=image->GetPixel(x+1,y)?5:0;
	matrix[5]=image->GetPixel(x+1,y+1)?6:0;
	matrix[6]=image->GetPixel(x,y+1)?7:0;
	matrix[7]=image->GetPixel(x-1,y+1)?8:0;

	if(matrix[7]&&matrix[0]) {
		matrix[0]=matrix[7];
	}
	for(int i=0; i<7; i++) {
		if(matrix[i+1] && matrix[i]) {
			matrix[i+1]=matrix[i];
		}
	}

	bool isUsed[8];
	for(int i=0; i<8; i++) {
		isUsed[i]=false;
	}
	for(int i=0; i<8; i++) {
		if(matrix[i]) {
			isUsed[matrix[i]-1]=true;
		}
	}
	for(int i=0; i<8; i++) {
		if(isUsed[i]) neighbors++;
	}

	return (neighbors==1)?true:false;
}

double CSkeletonizer::AFMMSolve(int i1, int l_j1, int i2, int j2, double sol, char* f, double* T, int width)
{
	double r,s;
	double newSol=sol;

	int p1=i1+l_j1*width;
	int p2=i2+j2*width;

	if(f[p1]==KNOWN) {
		if(f[p2]==KNOWN) {
			//average
			r = sqrt((2-(T[p1]-T[p2])*(T[p1]-T[p2])));
			s = (T[p1]+T[p2]-r)/2;
			if (s>=T[p1] && s>=T[p2]) {
				newSol=min(sol,s);
			} else {
				s += r;
				if ((s>=T[p1]) && (s>=T[p2])) newSol=min(sol,s);
			}
		}
		else {
			newSol=min(sol,1+T[p1]);
		}
	} else {
		if (f[p2]==KNOWN) {
			newSol=min(sol,1+T[p2]);
		}
	}
	return newSol;
}

void CSkeletonizer::TrackBoundary(int x, int y, char* f, double* U,double &val, int width)
{
	while(U[x+width*y]<0) {
		//TRACE("x,y: %i %i\n",x,y);
		U[x+width*y]=++val;

		if(f[x+1+y*width]==BAND && U[x+1+width*y]<0) {
			x++;
			continue;
		}
		if(f[x+(y+1)*width]==BAND && U[x+width*(1+y)]<0) {
			y++;
			continue;
		}
		if(f[x-1+width*y]==BAND && U[x-1+width*y]<0) {
			x--;
			continue;
		}
		if(f[x+width*(y-1)]==BAND && U[x+width*(y-1)]<0) {
			y--;
			continue;
		}

		if(f[x+1+width*(y+1)]==BAND && U[x+1+width*(y+1)]<0) {
			x++;
			y++;
			continue;
		}
		if(f[x-1+width*(y+1)]==BAND && U[x-1+(y+1)*width]<0) {
			y++;
			x--;
			continue;
		}
		if(f[x-1+width*(y-1)]==BAND && U[x-1+width*(y-1)]<0) {
			x--;
			y--;
			continue;
		}
		if(f[x+1+width*(y-1)]==BAND && U[x+1+width*(y-1)]<0) {
			y--;
			x++;
			continue;
		}
		break;
	}
}

/*
CRawImage<bool>* CSkeletonizer::MagnifyImage(CRawImage<bool>* img)
{
	int SCALE = int( CProjectSettings::Instance()->GetParam(
		CProjectSettings::SKELETONIZER_SCALE));

	CRawImage<bool> *tmp = new CRawImage<bool>(img->GetWidth()*SCALE, img->GetHeight()*SCALE);

	tmp->Fill();

	//blow up input image + 3 
	for(int x=0; x<tmp->GetWidth(); x+=SCALE) {
		for(int y=0; y<tmp->GetHeight(); y+=SCALE) {
			ARGB c=img->GetPixel(x/SCALE,y/SCALE);
			if(!c) {
				for(int i=0; i<SCALE; i++) {
					for(int j=0; j<SCALE; j++) {
						tmp->SetPixel(x+i,y+j,false);
					}
				}
			}
		}
	}

	//dilate tmp image to ensure connectivity in diagonal single-pixel lines
	
	//if(SCALE>1) {
	//	tmp->Dilate();
	//}

	return tmp;
}
*/
