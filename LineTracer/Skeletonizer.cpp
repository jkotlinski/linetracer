#include "StdAfx.h"
#include ".\skeletonizer.h"

#include "LineImageDrawer.h"
#include "EpsWriter.h"

#include <math.h>
#include <map>
#include <assert.h>

using namespace std;

CSkeletonizer* CSkeletonizer::_instance = 0;

CSkeletonizer::CSkeletonizer(void)
{
	SetParam("segmentSizeThreshold",5.0);
}

CSkeletonizer::~CSkeletonizer(void)
{
}

CSkeletonizer* CSkeletonizer::Instance() {
	if(_instance == 0) {
		_instance = new CSkeletonizer();
	}
	return _instance;
}

CRawImage* CSkeletonizer::Process(CRawImage *src) {
	CRawImage *distMap=new CRawImage(src->GetWidth(), src->GetHeight());
	distMap->Clear();

	DistanceTransform(src,distMap,3,4);
	
	CRawImage* maxMap = DeleteNonMaximums(distMap);

	CRawImage* knotImage=new CRawImage(src->GetWidth(),src->GetHeight());
	knotImage->Clear();

	CRawImage* segmentMap=CreateSegmentMap(maxMap,knotImage);

	CLineImage* li= Vectorize(segmentMap, knotImage);

	//segmentMap->Clear();
	CLineImageDrawer::Draw(segmentMap,li);

	li->SetWidth(knotImage->GetWidth());
	li->SetHeight(knotImage->GetHeight());

	CEpsWriter::Write(li);

	delete li;

	delete knotImage;
	delete distMap;

	delete maxMap;
	return segmentMap;

	//return maxMap;
	
}


void CSkeletonizer::DistanceTransform(CRawImage *src, CRawImage *dst, int DirectDistance, int IndirectDistance)
{
	//clean image
	for(int x=0; x<src->GetWidth()*src->GetHeight(); x++) {
		dst->SetPixel(x,0);
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

CRawImage* CSkeletonizer::DeleteNonMaximums(CRawImage* dst)
{
	CRawImage *tmp=new CRawImage(dst->GetWidth(), dst->GetHeight());

	for(int y=1; y<dst->GetHeight()-1; y++) {
		for(int x=1; x<dst->GetWidth()-1; x++) {
			bool isLocalMaximum=false;

			ARGB val=dst->GetPixel(x,y);

			//check absolute local maximum
			if(val>dst->GetPixel(x-1,y) && val>=dst->GetPixel(x+1,y)) {
				isLocalMaximum=true;
			} else if(val>dst->GetPixel(x,y-1) && val>=dst->GetPixel(x,y+1)) {
				isLocalMaximum=true;
			}

			tmp->SetPixel(x,y,isLocalMaximum?0xff:0);
		}
	}

	return tmp;
}

int CSkeletonizer::IsKnot(CRawImage* image, ARGB x, ARGB y)
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
CRawImage* CSkeletonizer::CreateSegmentMap(CRawImage* image, CRawImage* knotImage)
{
	map<int,int> labelMap;	
	map<int,int> labelCounter;	

	CRawImage* segmentMap=new CRawImage(image->GetWidth(),image->GetHeight());
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
				image->SetPixel(x,y,0);
			}
		}
	}

	int currentLabel=0;

	//set labels
	for(int y=1; y<image->GetHeight(); y++) {
		for(int x=1; x<image->GetWidth(); x++) {
			//if on a line
			if(image->GetPixel(x,y)&0xffff) {
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

CLineImage* CSkeletonizer::Vectorize(CRawImage* segmentImg, CRawImage* knotImg)
{
	CLineImage* li=new CLineImage();

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

				CSketchPoint startKnot=FindSegmentNeighbor(segmentImg, CPoint(x,y));

				while(startKnot.x!=-1) {
					CPolyLine* line=new CPolyLine();

					line->Add(CSketchPoint(x,y,true,true));

					TraceLine(segmentImg,knotImg,line,CPoint(startKnot.x,startKnot.y),&forbiddenEndKnot);

					li->Add(line);

					startKnot=FindSegmentNeighbor(segmentImg, CPoint(x,y));
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
				if(p.x!=-1) {
					CPolyLine *line=new CPolyLine();
					line->Add(CSketchPoint(x,y,true,true));
					line->Add(p);
					li->Add(line);

					//clear knot
					knotImg->SetPixel(x,y,0);
				}
			}
		}
	}

	return li;
}

void CSkeletonizer::TraceLine(CRawImage* segmentImage, CRawImage* knotImage, CPolyLine* line, CPoint start, map<int,bool> *forbiddenEndKnotIds)
{
	CSketchPoint p(start);

	CPoint endKnot=IsKnotNeighbor(knotImage, p, forbiddenEndKnotIds);

	if(endKnot.x!=-1 && NoOrthogonalNeighbors(segmentImage, p)) {
		//point is a knot neighbor
		//add point
		line->Add(p);

		//clear point
		segmentImage->SetPixel(p.x,p.y,0);

		//add neighbor knot
		p=CSketchPoint(endKnot,true,true);
		line->Add(p);
		return;
	}

	while(1) {
		assert(p.x>=-1);
		assert(p.y>=-1);

		//get neigbor point
		CSketchPoint p_new(FindSegmentNeighbor(segmentImage,p));

		if(p_new.x==-1) {
			//add point as endpoint
			p.SetIsEndPoint(true);
			line->Add(p);

			//clear point
			segmentImage->SetPixel(p.x,p.y,0);

			//reached end of line
			break;
		}

		//add point to line
		line->Add(p);

		assert(p_new.x>=-1);
		assert(p_new.y>=-1);

		//clear old pixel
		segmentImage->SetPixel(p.x,p.y,0);

		endKnot=IsKnotNeighbor(knotImage, p_new, forbiddenEndKnotIds);

		if(endKnot.x!=-1 && NoOrthogonalNeighbors(segmentImage, p_new)) {
			//point is a knot neighbor
			//add point
			line->Add(p_new);

			//clear point
			segmentImage->SetPixel(p_new.x,p_new.y,0);

			//add neighbor knot
			p=CSketchPoint(endKnot,true,true);
			line->Add(p);
			break;
		}
		p=p_new;
	}
}

/* return true if the point is a knot neighbor */
CPoint CSkeletonizer::IsKnotNeighbor(CRawImage* knotImage, CPoint point, map<int,bool> *forbiddenEndKnotIds)
{
	int x=point.x;
	int y=point.y;

	if(!(knotImage->GetPixel(x,y)&0x40000000)) return CPoint(-1,-1);

	int p;

	p=knotImage->GetPixel(x-1,y);
	if((p&0x80000000) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CPoint(x-1,y);
	p=knotImage->GetPixel(x+1,y);
	if((p&0x80000000) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CPoint(x+1,y);
	p=knotImage->GetPixel(x,y-1);
	if((p&0x80000000) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CPoint(x,y-1);
	p=knotImage->GetPixel(x,y+1);
	if((p&0x80000000) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CPoint(x,y+1);

	p=knotImage->GetPixel(x+1,y+1);
	if((p&0x80000000) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CPoint(x+1,y+1);
	p=knotImage->GetPixel(x+1,y-1);
	if((p&0x80000000) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CPoint(x+1,y-1);
	p=knotImage->GetPixel(x-1,y+1);
	if((p&0x80000000) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CPoint(x-1,y+1);
	p=knotImage->GetPixel(x-1,y-1);
	if((p&0x80000000) && !(*forbiddenEndKnotIds)[p&0xffffff]) return CPoint(x-1,y-1);

	return CPoint(-1,-1);
}

/* return the position of any knot neighboring p */
CSketchPoint CSkeletonizer::FindNeighborKnot(CRawImage* knotImg, CPoint p)
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

	return CSketchPoint(CPoint(-1,-1));
}

CPoint CSkeletonizer::FindSegmentNeighbor(CRawImage* segmentImage, CPoint p)
{
	//check orthogonal neighbors
	if(segmentImage->GetPixel(p.x-1,p.y)) return CPoint(p.x-1,p.y);
	if(segmentImage->GetPixel(p.x,p.y-1)) return CPoint(p.x,p.y-1);
	if(segmentImage->GetPixel(p.x+1,p.y)) return CPoint(p.x+1,p.y);
	if(segmentImage->GetPixel(p.x,p.y+1)) return CPoint(p.x,p.y+1);

	//check diagonal neighbors
	int seg = segmentImage->GetPixel(p.x,p.y);

	if(seg) {
		//current point is a normal line point
		if(segmentImage->GetPixel(p.x-1,p.y-1)==seg) return CPoint(p.x-1,p.y-1);
		if(segmentImage->GetPixel(p.x+1,p.y-1)==seg) return CPoint(p.x+1,p.y-1);
		if(segmentImage->GetPixel(p.x+1,p.y+1)==seg) return CPoint(p.x+1,p.y+1);
		if(segmentImage->GetPixel(p.x-1,p.y+1)==seg) return CPoint(p.x-1,p.y+1);
	} else {
		//current point is a knot. return any segment
		if(segmentImage->GetPixel(p.x-1,p.y-1)) return CPoint(p.x-1,p.y-1);
		if(segmentImage->GetPixel(p.x+1,p.y-1)) return CPoint(p.x+1,p.y-1);
		if(segmentImage->GetPixel(p.x+1,p.y+1)) return CPoint(p.x+1,p.y+1);
		if(segmentImage->GetPixel(p.x-1,p.y+1)) return CPoint(p.x-1,p.y+1);
	}

	return CPoint(-1,-1);
}

bool CSkeletonizer::NoOrthogonalNeighbors(CRawImage* segmentImage, CPoint p)
{
	//check horizontal+vertical neighbors
	if(segmentImage->GetPixel(p.x-1,p.y)) return false;
	if(segmentImage->GetPixel(p.x,p.y-1)) return false;
	if(segmentImage->GetPixel(p.x+1,p.y)) return false;
	if(segmentImage->GetPixel(p.x,p.y+1)) return false;
	return true;
}
