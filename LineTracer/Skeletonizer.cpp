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

//#define DEFINE_BLOCK_ALLOCATED_MAP
//#define DEFINE_BLOCK_ALLOCATED_MULTIMAP
//#include "blockallocator.h"

CSkeletonizer::CSkeletonizer(void)
: CImageProcessor()
{
	SetName ( CString ( "Skeletonizer" ) );
	CProjectSettings::Instance()->SetParam( CProjectSettings::SKELETONIZER_SCALE, 1.0 );
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

	CRawImage<ARGB>* segmentMap=CreateSegmentMap(src,knotImage);

	CLineImage* li = Vectorize(segmentMap, knotImage);

	delete knotImage;

	//trace endpoints
	TraceEndpoints ( *segmentMap, *li );

	//TODO: circle detect
	//we have a bunch of segments left, that have no end points.
	TraceCircles ( *segmentMap, *li );

	delete segmentMap;

	LOG("skeletonizer->process complete\n");
	li->UpdateTailData();
	CLineImage* li2 = li->SmoothPositions();
	delete li;
	return li2;
}

void CSkeletonizer::TraceEndpoints ( CRawImage<ARGB> &segmentMap, CLineImage &li )
{
	for(int x=1; x<segmentMap.GetWidth()-1; x++) {
		for(int y=1; y<segmentMap.GetHeight()-1; y++) {
			if(segmentMap.GetPixel(x,y) && IsEndPoint(&segmentMap, CPoint(x,y))) {
				CPoint p,p_prev;
				CPolyLine *line = new CPolyLine();
				//add first point
				line->Add(new CSketchPoint(x,y,true));
				segmentMap.SetPixel(x,y,0);
				p_prev=FindSegmentNeighbor(&segmentMap,CPoint(x,y));

				while( true ) 
				{
					p=FindSegmentNeighbor(&segmentMap,p_prev);
					if ( p.x == -1 ) 
					{
						//add last point
						line->Add(new CSketchPoint(p_prev.x,p_prev.y,true));
						segmentMap.SetPixel(int(p_prev.x),int(p_prev.y),0);
						break;
					}
					line->Add(new CSketchPoint(p_prev.x, p_prev.y));
					segmentMap.SetPixel(int(p_prev.x),int(p_prev.y),0);
					p_prev=p;
				}
				li.Add(line);
			}
		}
	}
}

void CSkeletonizer::TraceCircles( CRawImage<ARGB> &segmentMap, CLineImage &li )
{
	for(int y=1; y<segmentMap.GetHeight()-1; y++) 
	{
		for(int x=1; x<segmentMap.GetWidth()-1; x++) 
		{
			if(segmentMap.GetPixel(x,y)) 
			{
				CPolyLine *l_circleLine = new CPolyLine();
				l_circleLine->SetIsCircle();

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

/*typedef boost::fast_pool_allocator<pair<double,int> > NBAllocator;
typedef multimap<double,int,less<double>, NBAllocator > NBType;

MaximumMapType* CSkeletonizer::DoAFMM(CRawImage<bool>* src, bool direction) {
	static const double MAX_VALUE=1000000;
	static const double THRESHOLD = 8;
	static const bool paintU = true;

	map<int,NBType::iterator,less<int>,boost::fast_pool_allocator<pair<int,NBType::iterator> > > ptr;
	NBType NarrowBand;

	int width=src->GetWidth();

	MaximumMapType* maximumMap = new MaximumMapType();
	char *f = new char[src->GetHeight()*src->GetWidth()];
	double *T = new double[src->GetHeight()*src->GetWidth()];
	double *U = new double[src->GetHeight()*src->GetWidth()];

	TRACE("do AFMM\n");

	for(int x=0; x<src->GetWidth(); x++) {
		f[x]=KNOWN;
		f[x+width*(src->GetHeight()-1)]=KNOWN;
	}
	for(int y=0; y<src->GetHeight(); y++) {
		f[y*width]=KNOWN;
		f[src->GetWidth()-1+width*y]=KNOWN;
	}

	//initialization
	for(int x=1; x<src->GetWidth()-1; x++) {
		for(int y=1; y<src->GetHeight()-1; y++) {
			if(src->GetPixel(x,y)) {
				//outside boundary
				f[x+width*y]=KNOWN;
				T[x+width*y]=0;
				U[x+width*y]=0;
				continue;
			}

			//on boundary?
			bool onBoundary=false;
			if(src->GetPixel(x-1,y) ||
				src->GetPixel(x+1,y) ||
				src->GetPixel(x,y-1) ||
				src->GetPixel(x,y+1)) onBoundary=true;

			if(onBoundary) {
				f[x+y*width]=BAND;
				T[x+width*y]=0;
				U[x+width*y]=-1;
				ptr[(x<<16)|y]=
					NarrowBand.insert(pair<double,int>(0,(x<<16)|y));
				//ptrSet[(x<<16)|y]=true;
				//TRACE("%x\n",ptr[(x<<16)|(y&0xffff)]);
			} else {
				f[x+y*width]=INSIDE;
				T[x+width*y]=MAX_VALUE;
			}
		}
	}

	TRACE("boundary done\n");
	TRACE("i NarrowBand.size(): %i\n",NarrowBand.size());

	double currU=0;

	TRACE("do track boundary\n");

	//initialize U val on boundary
	if(direction) {
		for(int x=1; x<src->GetWidth()-1; x++) {
			for(int y=1; y<src->GetHeight()-1; y++) {
				int p=x+y*width;

				if(f[p]==BAND && U[p]<0) {
					TrackBoundary(x,y,f,U,currU,width);
				}
			}
		}
	} else {
		for(int x=src->GetWidth()-2; x>0; x--) {
			for(int y=src->GetHeight()-2; y>0; y--) {
				int p=x+y*width;

				if(f[p]==BAND && U[p]<0) {
					TrackBoundary(x,y,f,U,currU,width);
				}
			}
		}
	}

	TRACE("U boundary val inited\n");

	//main process
	while(NarrowBand.size()) {
		//extract first element from heap
		int point = (*NarrowBand.begin()).second;
		CPoint p(point>>16, point&0xffff);
		NarrowBand.erase(NarrowBand.begin());
		f[p.GetX()+p.GetY()*width]=KNOWN;

		if(NarrowBand.size()%100==0) {
			TRACE("m NarrowBand.size(): %i\n",NarrowBand.size());
		}

		int checkedNeighbors=0;

		//check neighbors
		for(int i=0; i<4; i++) {
			int x = p.GetX();
			int y = p.GetY();
			switch(i) {
				case 0:
					x--;
					break;
				case 1:
					x++;
					break;
				case 2:
					y--;
					break;
				case 3:
					y++;
					break;
			}

			if(f[x+y*width]!=KNOWN) {
				//march inwards, add point
				if(f[x+y*width]==INSIDE) {
					checkedNeighbors++;
					f[x+y*width]=BAND;

					//propagate U
					int knownNeighbors = 0;
					double a=0,m=1000000,M=-1;
					int pp = x-1+y*width;
					if(f[pp]==KNOWN) {
						knownNeighbors++;
						m=min(m,U[pp]);
						M=max(M,U[pp]);
						a+=U[pp];
					}
					pp=x+1+y*width;
					if(f[pp]==KNOWN) {
						knownNeighbors++;
						m=min(m,U[pp]);
						M=max(M,U[pp]);
						a+=U[pp];
					}
					pp=x+(y-1)*width;
					if(f[pp]==KNOWN) {
						m=min(m,U[pp]);
						M=max(M,U[pp]);
						a+=U[pp];
						knownNeighbors++;
					}
					pp=x+(y+1)*width;
					if(f[pp]==KNOWN) {
						m=min(m,U[pp]);
						M=max(M,U[pp]);
						a+=U[pp];
						knownNeighbors++;
					}
					if(m-M<2) {
						U[x+y*width]=a;
					} else {
						U[x+y*width]=m;
					}
				}

				//compute T
				double sol=MAX_VALUE;
				sol=AFMMSolve(x-1,y,x,y-1,sol, f, T, width);
				sol=AFMMSolve(x+1,y,x,y-1,sol, f, T, width);
				sol=AFMMSolve(x-1,y,x,y+1,sol, f, T, width);
				sol=AFMMSolve(x+1,y,x,y+1,sol, f, T, width);
				T[x+width*y]=sol;

				//TRACE("erase %x\n",ptr[(x<<16)|(y&0xffff)]);
				if(ptr[(x<<16)|y]!=0) {
					NarrowBand.erase(ptr[(x<<16)|y]);
				}
				ptr[(x<<16)|y]=
					NarrowBand.insert(pair<double,int>(sol,(x<<16)|y));
			}
		}

		if(!checkedNeighbors) {
			(*maximumMap)[p.GetX()+width*p.GetY()]=1;
		}
	}

	//CRawImage<short> *dst = new CRawImage<short>(src->GetWidth(), src->GetHeight());
	//dst->Clear();

	TRACE("main afmm complete\n");
	
	for(int y=1; y<src->GetHeight()-1; y++) {
		int counter=width*y;
		for(int x=1; x<src->GetWidth()-1; x++) {
			counter++;
			if(U[counter]>0.5) {
				if(U[counter+1]>0.5) {
					if(abs(U[counter]-U[counter+1])>=THRESHOLD) {
						if((*maximumMap)[counter]) {
							(*maximumMap)[counter]=0x8000|short(T[counter]);
						} else {
							(*maximumMap)[counter]=0x4000|short(T[counter]);
						}
						continue;
					} 
				}
				if(U[counter+width]>0.5) {
					if(abs(U[counter]-U[counter+width])>=THRESHOLD) {
						if((*maximumMap)[counter]) {
							(*maximumMap)[counter]=0x8000|short(T[counter]);
						} else {
							(*maximumMap)[counter]=0x4000|short(T[counter]);
						}
						continue;
					} 
				}
				(*maximumMap)[counter]=0;
			}
			//if(paintU) dst->SetPixel(x,y,short(U[x+width*y]));
		}
	}

	TRACE("painted tmp\n");
	delete f;
	delete T;
	delete U;

	return maximumMap;
}*/

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

int CSkeletonizer::IsKnot(CRawImage<bool>* image, ARGB x, ARGB y)
{
	static int currentKnot = 1;

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

	return (neighbors>2)?currentKnot++:0;
}


/*one-pass segmentation algorithm*/
CRawImage<ARGB>* CSkeletonizer::CreateSegmentMap(CRawImage<bool>* image, CRawImage<ARGB>* knotImage)
{
	map<int,int> labelMap;	
	map<int,int> labelCounter;	

	CRawImage<ARGB>* segmentMap=new CRawImage<ARGB>(image->GetWidth(),image->GetHeight());
	segmentMap->Clear();

	//paint knots red
	for(int x=1; x<image->GetWidth()-1; x++) {
		for(int y=1; y<image->GetHeight()-1; y++) {
	
			if(image->GetPixel(x,y)) {
				int knot = IsKnot(image,x,y);
				if(knot) {
					//mark knot
					knotImage->SetPixel(x,y,    0x80000000|knot);

					//remove knot from image map
					//image->SetPixel(x,y,0);

					//mark knot neighbors
					if(!(knotImage->GetPixel(x-1,y)&0x80000000)) {
						knotImage->SetPixel(x-1,y,  0x40000000|knot);
					}
					if(!(knotImage->GetPixel(x+1,y)&0x80000000)) {
						knotImage->SetPixel(x+1,y,  0x40000000|knot);
					}
					if(!(knotImage->GetPixel(x,y+1)&0x80000000)) {
						knotImage->SetPixel(x,y+1,  0x40000000|knot);
					}
					if(!(knotImage->GetPixel(x,y-1)&0x80000000)) {
						knotImage->SetPixel(x,y-1,  0x40000000|knot);
					}
					if(!(knotImage->GetPixel(x-1,y-1)&0x80000000)) {
						knotImage->SetPixel(x-1,y-1,  0x40000000|knot);
					}
					if(!(knotImage->GetPixel(x-1,y+1)&0x80000000)) {
						knotImage->SetPixel(x-1,y+1,  0x40000000|knot);
					}
					if(!(knotImage->GetPixel(x+1,y-1)&0x80000000)) {
						knotImage->SetPixel(x+1,y-1,  0x40000000|knot);
					}
					if(!(knotImage->GetPixel(x+1,y+1)&0x80000000)) {
						knotImage->SetPixel(x+1,y+1,  0x40000000|knot);
					}
				}
			}
		}
	}

	//remove knots from image map
	for(int x=1; x<image->GetWidth()-1; x++) {
		for(int y=1; y<image->GetHeight()-1; y++) {
			if(knotImage->GetPixel(x,y)&0x80000000) {
				image->SetPixel(x,y,false);
			}
		}
	}

	int currentLabel=0;

	//set labels
	for(int y=1; y<image->GetHeight(); y++) {
		for(int x=1; x<image->GetWidth(); x++) {
			//if on a line
			if(image->GetPixel(x,y)) {
				ARGB p1=segmentMap->GetPixel(x-1,y);
				ARGB p2=segmentMap->GetPixel(x-1,y-1);
				ARGB p3=segmentMap->GetPixel(x,y-1);
				ARGB p4=segmentMap->GetPixel(x+1,y-1);

				int p1knotId=knotImage->GetPixel(x-1,y);
				int p2knotId=knotImage->GetPixel(x-1,y-1);
				int p3knotId=knotImage->GetPixel(x,y-1);
				int p4knotId=knotImage->GetPixel(x+1,y-1);

				if(p1knotId&0x40000000) p1knotId&=0xfffffff;
				if(p2knotId&0x40000000) p2knotId&=0xfffffff;
				if(p3knotId&0x40000000) p3knotId&=0xfffffff;
				if(p4knotId&0x40000000) p4knotId&=0xfffffff;

				int myKnotId=knotImage->GetPixel(x,y);
				if(myKnotId&0x40000000) myKnotId&=0xfffffff;

				if(myKnotId) {
					if(myKnotId==p1knotId) p1=0;
					if(myKnotId==p2knotId) p2=0;
					if(myKnotId==p3knotId) p3=0;
					if(myKnotId==p4knotId) p4=0;
				}

				//all neighbors empty?
				if(!p1 && !p2 && !p3 && !p4) {
					segmentMap->SetPixel(x,y,++currentLabel);
					continue;
				}
				
				//some neighbor not empty

				//check for conflicts
				bool conflict=false;
				if(p1 && p2 && (p1!=p2)) conflict=true;
				if(p1 && p3 && (p1!=p3)) conflict=true;
				if(p1 && p4 && (p1!=p4)) conflict=true;
				if(p2 && p3 && (p2!=p3)) conflict=true;
				if(p2 && p4 && (p2!=p4)) conflict=true;
				if(p3 && p4 && (p3!=p4)) conflict=true;

				ARGB useLabel=0xffffff;

				if(!conflict) {
					if(p1) useLabel=p1;
					if(p2) useLabel=p2;
					if(p3) useLabel=p3;
					if(p4) useLabel=p4;
				} else {
					//conflict!

					//get min
					useLabel=0xffffffff;

					if(p1 && p2 && (p1!=p2) && (!p1knotId || !p2knotId || p1knotId!=p2knotId)) {
						useLabel=min(p1,p2);
						labelMap[max(p1,p2)]=useLabel;
					}
					if(p1 && p3 && (p1!=p3) && (!p1knotId || !p3knotId || p1knotId!=p3knotId)) {
						useLabel=min(p1,p3);
						labelMap[max(p1,p3)]=useLabel;
					}
					if(p1 && p4 && (p1!=p4) && (!p1knotId || !p4knotId || p1knotId!=p4knotId)) {
						useLabel=min(p1,p4);
						labelMap[max(p1,p4)]=useLabel;
					}
					if(p2 && p3 && (p2!=p3) && (!p2knotId || !p3knotId || p2knotId!=p3knotId)) {
						useLabel=min(p2,p3);
						labelMap[max(p2,p3)]=useLabel;
					}
					if(p2 && p4 && (p2!=p4) && (!p2knotId || !p4knotId || p2knotId!=p4knotId)) {
						useLabel=min(p2,p4);
						labelMap[max(p2,p4)]=useLabel;
					}
					if(p3 && p4 && (p3!=p4) && (!p3knotId || !p4knotId || p3knotId!=p4knotId)) {
						useLabel=min(p3,p4);
						labelMap[max(p3,p4)]=useLabel;
					}
				}
				segmentMap->SetPixel(x,y,useLabel);
			}
		}
	}

	//build equivalence class
	int counter=0;
	for(int i=1; i<=currentLabel; i++) {
		if(labelMap[i]==0) {
			labelMap[i]=++counter;
		} else {
			labelMap[i]=labelMap[labelMap[i]];
		}
	}

	//combine equivalent segments
	for(int i=0; i<image->GetHeight()*image->GetWidth(); i++) {
		if(image->GetPixel(i)) {
			int segment=labelMap[segmentMap->GetPixel(i)];

			segmentMap->SetPixel(i,segment);
			labelCounter[segment]=labelCounter[segment]+1;
		}
	}

	return segmentMap;
}

CLineImage* CSkeletonizer::Vectorize(CRawImage<ARGB>* segmentImg, CRawImage<ARGB>* knotImg)
{
	CLineImage* li=new CLineImage(knotImg->GetWidth(), knotImg->GetHeight());

	//trace lines with knot endpoints
	for(int y=1; y<knotImg->GetHeight()-1; y++) {
		for(int x=1; x<knotImg->GetWidth()-1; x++) {
			if(knotImg->GetPixel(x,y)&0x80000000) {
				//is a knot

				//find forbidden end knots
				map<int,bool> forbiddenEndKnot;
				forbiddenEndKnot[knotImg->GetPixel(x,y)&0xffffff]=true;
				int p;
				p=knotImg->GetPixel(x+1,y);
				if(p&0x80000000) forbiddenEndKnot[p&0xffffff]=true;
				p=knotImg->GetPixel(x-1,y);
				if(p&0x80000000) forbiddenEndKnot[p&0xffffff]=true;
				p=knotImg->GetPixel(x,y+1);
				if(p&0x80000000) forbiddenEndKnot[p&0xffffff]=true;
				p=knotImg->GetPixel(x,y-1);
				if(p&0x80000000) forbiddenEndKnot[p&0xffffff]=true;

				p=knotImg->GetPixel(x-1,y-1);
				if(p&0x80000000) forbiddenEndKnot[p&0xffffff]=true;
				p=knotImg->GetPixel(x-1,y+1);
				if(p&0x80000000) forbiddenEndKnot[p&0xffffff]=true;
				p=knotImg->GetPixel(x+1,y-1);
				if(p&0x80000000) forbiddenEndKnot[p&0xffffff]=true;
				p=knotImg->GetPixel(x+1,y+1);
				if(p&0x80000000) forbiddenEndKnot[p&0xffffff]=true;

				CPoint l_startPoint = FindSegmentNeighbor(segmentImg, CPoint(x,y));
				CSketchPoint startKnot ( l_startPoint.x, l_startPoint.y );

				while(startKnot.GetX() != -1) {
					CPolyLine* line=new CPolyLine();

					line->Add(new CSketchPoint(x,y,true,true));

					TraceLine(segmentImg,knotImg,line,startKnot.GetCoords(),&forbiddenEndKnot);

					li->Add(line);

					CPoint l_newPoint = FindSegmentNeighbor(segmentImg, CPoint(x,y));
					startKnot = CSketchPoint( l_newPoint.x, l_newPoint.y );
				}
			}
		}
	}

	//connect neighboring knots
	for(int x=1; x<knotImg->GetWidth()-1; x++) {
		for(int y=1; y<knotImg->GetHeight()-1; y++) {
			if(knotImg->GetPixel(x,y)&0x80000000) {
				CSketchPoint p=FindNeighborKnot(knotImg, CPoint(x,y));

				//found neighbor
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

void CSkeletonizer::TraceLine(CRawImage<ARGB>* segmentImage, CRawImage<ARGB>* knotImage, CPolyLine* line, CFPoint start, map<int,bool> *forbiddenEndKnotIds)
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

	if(!(knotImage->GetPixel((int)x,(int)y)&0x40000000)) return CFPoint(-1,-1);

	int p;

	p=knotImage->GetPixel(int(x-1),(int)y);
	if((p&0x80000000) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CFPoint(x-1,y);
	p=knotImage->GetPixel(int(x+1),int(y));
	if((p&0x80000000) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CFPoint(x+1,y);
	p=knotImage->GetPixel(int(x),int(y-1));
	if((p&0x80000000) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CFPoint(x,y-1);
	p=knotImage->GetPixel(int(x),int(y+1));
	if((p&0x80000000) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CFPoint(x,y+1);

	p=knotImage->GetPixel(int(x+1),int(y+1));
	if((p&0x80000000) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CFPoint(x+1,y+1);
	p=knotImage->GetPixel(int(x+1),int(y-1));
	if((p&0x80000000) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CFPoint(x+1,y-1);
	p=knotImage->GetPixel(int(x-1),int(y+1));
	if((p&0x80000000) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CFPoint(x-1,y+1);
	p=knotImage->GetPixel(int(x-1),int(y-1));
	if((p&0x80000000) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CFPoint(x-1,y-1);

	return CFPoint(-1,-1);
}

/* return the position of any knot neighboring p */
CSketchPoint CSkeletonizer::FindNeighborKnot(CRawImage<ARGB>* knotImg, CPoint p)
{
	//check horizontal+vertical neighbors
	if(knotImg->GetPixel(p.x-1,p.y)&0x80000000) return CSketchPoint(p.x-1,p.y,true,true);
	if(knotImg->GetPixel(p.x,p.y-1)&0x80000000) return CSketchPoint(p.x,p.y-1,true,true);
	if(knotImg->GetPixel(p.x+1,p.y)&0x80000000) return CSketchPoint(p.x+1,p.y,true,true);
	if(knotImg->GetPixel(p.x,p.y+1)&0x80000000) return CSketchPoint(p.x,p.y+1,true,true);

	//check diagonal neighbors
	if(knotImg->GetPixel(p.x-1,p.y-1)&0x80000000) return CSketchPoint(p.x-1,p.y-1,true,true);
	if(knotImg->GetPixel(p.x+1,p.y-1)&0x80000000) return CSketchPoint(p.x+1,p.y-1,true,true);
	if(knotImg->GetPixel(p.x+1,p.y+1)&0x80000000) return CSketchPoint(p.x+1,p.y+1,true,true);
	if(knotImg->GetPixel(p.x-1,p.y+1)&0x80000000) return CSketchPoint(p.x-1,p.y+1,true,true);

	return CSketchPoint(CFPoint(-1,-1));
}

CPoint CSkeletonizer::FindSegmentNeighbor(CRawImage<ARGB>* segmentImage, const CPoint & p)
{
	if(!p.x || p.x==segmentImage->GetWidth()-1) return CPoint(-1,-1);
	if(!p.y || p.y==segmentImage->GetHeight()-1) return CPoint(-1,-1);

	//check orthogonal neighbors
	if(segmentImage->GetPixel(p.x-1,p.y)) return CPoint(p.x-1,p.y);
	if(segmentImage->GetPixel(p.x,p.y-1)) return CPoint(p.x,p.y-1);
	if(segmentImage->GetPixel(p.x+1,p.y)) return CPoint(p.x+1,p.y);
	if(segmentImage->GetPixel(p.x,p.y+1)) return CPoint(p.x,p.y+1);

	//check diagonal neighbors
	int seg = segmentImage->GetPixel((int)p.x,(int)p.y);

	if(seg) {
		//current point is a normal line point
		if(segmentImage->GetPixel(int(p.x-1),int(p.y-1))==seg) return CPoint(p.x-1,p.y-1);
		if(segmentImage->GetPixel(int(p.x+1),int(p.y-1))==seg) return CPoint(p.x+1,p.y-1);
		if(segmentImage->GetPixel(int(p.x+1),int(p.y+1))==seg) return CPoint(p.x+1,p.y+1);
		if(segmentImage->GetPixel(int(p.x-1),int(p.y+1))==seg) return CPoint(p.x-1,p.y+1);
	} else {
		//current point is a knot. return any segment
		if(segmentImage->GetPixel(int(p.x-1),int(p.y-1))) return CPoint(p.x-1,p.y-1);
		if(segmentImage->GetPixel(int(p.x+1),int(p.y-1))) return CPoint(p.x+1,p.y-1);
		if(segmentImage->GetPixel(int(p.x+1),int(p.y+1))) return CPoint(p.x+1,p.y+1);
		if(segmentImage->GetPixel(int(p.x-1),int(p.y+1))) return CPoint(p.x-1,p.y+1);
	}

	return CPoint(-1,-1);
}

bool CSkeletonizer::NoOrthogonalNeighbors(CRawImage<ARGB>* segmentImage, CFPoint p)
{
	//check horizontal+vertical neighbors
	if(segmentImage->GetPixel(int(p.GetX()-1),(int)p.GetY())) return false;
	if(segmentImage->GetPixel((int)p.GetX(),int(p.GetY()-1))) return false;
	if(segmentImage->GetPixel(int(p.GetX()+1),(int)p.GetY())) return false;
	if(segmentImage->GetPixel((int)p.GetX(),int(p.GetY()+1))) return false;
	return true;
}

bool CSkeletonizer::IsEndPoint(CRawImage<ARGB>* image, CPoint p)
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
