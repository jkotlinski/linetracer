#include "StdAfx.h"
#include ".\forkhandler.h"

#include "FPoint.h"
#include "PolyLine.h"

#include <math.h>
#include <vector>
#include <map>
#include <assert.h>

#include "Binarizer.h"

CForkHandler::CForkHandler(void)
{
	ThreeForks = new vector<CFPoint>;
	TForks = new vector<CFPoint>;
	YForks = new vector<CFPoint>;
}

CForkHandler::~CForkHandler(void)
{
	delete ThreeForks;
	delete TForks;
	delete YForks;
}

CForkHandler* CForkHandler::Instance() {
    static CForkHandler inst;
    return &inst;
}

CSketchImage* CForkHandler::Process(CSketchImage* i_src)
{
	CLineImage *src = static_cast<CLineImage*>(i_src);

	delete ThreeForks;
	ThreeForks = Find3Forks(src);

	CLineImage *tmp = HandleTForks(src);
	//causes memory leak!!!
	CLineImage *tmp2 = HandleYForks(tmp);
	
	delete tmp;
	return tmp2;
}

vector<CFPoint>* CForkHandler::Find3Forks(CLineImage *li)
{
	map<CFPoint,int> counter;
	vector<CFPoint>* forks = new vector<CFPoint>;

	for(int i=0; i<li->Size(); i++) {
		CPolyLine *line = li->At(i);

		if(*(line->GetHeadPoint()) != *(line->GetTailPoint())) {
			counter[*(line->GetHeadPoint())] = counter[*(line->GetHeadPoint())] + 1;
			counter[*(line->GetTailPoint())] = counter[*(line->GetTailPoint())] + 1;
		} else {
			counter[*(line->GetHeadPoint())] = -10000;
			TRACE("WARNING: line headpoint equals tailpoint!\n");
		}
	}

	map<CFPoint,int>::iterator iter;

	for(iter=counter.begin(); iter!=counter.end(); iter++) {
		if((*iter).second==3) {
			forks->push_back((*iter).first);
			TRACE("threefork: %f %f\n",(*iter).first.x,(*iter).first.y);
		}
	}
	return forks;
}

CLineImage* CForkHandler::HandleTForks(CLineImage* li)
{
	static const double SAME_LINE_MAX = -0.8;
	static const double CROSS_LINE_MIN = -0.75;

	CLineImage *tmpLi = li->Clone();
	CLineImage *newImage = new CLineImage(li->GetWidth(),li->GetHeight());

	delete TForks;
	TForks = new vector<CFPoint>;

	vector<CFPoint>::iterator iter;
	map<int,bool> dontAddLine;

	for(iter=ThreeForks->begin(); iter!=ThreeForks->end(); iter++) {
		//iterate through all three-fork points

		vector<CFPoint> vectors;
		vector<int> lineId;

		TRACE("tmpLi->Size(): %i\n",tmpLi->Size());
		for(int i=0; i<tmpLi->Size(); i++) {
			if(dontAddLine[i]) continue;
			CPolyLine *line = tmpLi->At(i);

			if(*(line->GetHeadPoint()) == *iter) {
				//fork point == line headpoint
				int j = min(2, line->Size()-1);
				CFPoint p = *line->At(j) - *line->At(j-1);
				vectors.push_back(p.Unit());
				lineId.push_back(i);
			} else if(*(line->GetTailPoint()) == *iter) {
				//fork point == line tailpoint
				int j = max(0, line->Size()-3);
				CFPoint p = *line->At(j) - *line->At(j+1);
				vectors.push_back(p.Unit());
				lineId.push_back(i);
			}
		}

		if(vectors.size()!=3) continue;
		TRACE("vectors.size(): %i\n",vectors.size());
		assert(vectors.size()==3);
		assert(lineId.size()==3);

		//calc cross products
		double C_01 = vectors[0] * vectors[1];
		double C_02 = vectors[0] * vectors[2];
		double C_12 = vectors[1] * vectors[2];

		//TRACE("C_01: %f\n",C_01);
		//TRACE("C_02: %f\n",C_02);
		//TRACE("C_12: %f\n",C_12);

		tmpLi->IsTail(tmpLi->At(lineId[0]));

		if(C_01 < SAME_LINE_MAX && C_02 > CROSS_LINE_MIN && C_12 > CROSS_LINE_MIN) {
			//merge line 0 and 1
			TForks->push_back(*iter);
			dontAddLine[lineId[0]]=true;
			dontAddLine[lineId[1]]=true;
			CPolyLine *l1 = tmpLi->At(lineId[0])->Clone();
			CPolyLine *l2 = tmpLi->At(lineId[1])->Clone();
			CPolyLine *l3 = l1->MergeLine(l2);
			CPolyLine *l4 = l3->Clone();
			int l1Size = l1->Size();
			SmoothLine(l4,l1Size-1);
			//SetEndPoint(tmpLi->At(lineId[2]),p);
			tmpLi->Add(l4);
			delete l1;
			delete l2;
			delete l3;
			//tmpLi->Add(l2);
			TRACE("fork. don't add lines %i %i\n",lineId[0],lineId[1]);
		} else if(C_02 < SAME_LINE_MAX && C_01 > CROSS_LINE_MIN && C_12 > CROSS_LINE_MIN) {
			//merge line 0 and 2
			TForks->push_back(*iter);
			dontAddLine[lineId[0]]=true;
			dontAddLine[lineId[2]]=true;
			CPolyLine *l1 = tmpLi->At(lineId[0])->Clone();
			CPolyLine *l2 = tmpLi->At(lineId[2])->Clone();
			CPolyLine *l3 = l1->MergeLine(l2);
			CPolyLine *l4 = l3->Clone();
			int l1Size = l1->Size();
			SmoothLine(l4,l1Size-1);
			//SetEndPoint(tmpLi->At(lineId[1]),p);
			tmpLi->Add(l4);
			delete l1;
			delete l2;
			delete l3;
			TRACE("fork. don't add lines %i %i\n",lineId[0],lineId[2]);
		} else if(C_12 < SAME_LINE_MAX && C_01 > CROSS_LINE_MIN && C_02 > CROSS_LINE_MIN) {
			//merge line 1 and 2
			TForks->push_back(*iter);
			dontAddLine[lineId[1]]=true;
			dontAddLine[lineId[2]]=true;
			CPolyLine *l1 = tmpLi->At(lineId[1])->Clone();
			CPolyLine *l2 = tmpLi->At(lineId[2])->Clone();
			CPolyLine *l3 = l1->MergeLine(l2);
			CPolyLine *l4 = l3->Clone();
			int l1Size = l1->Size();
			SmoothLine(l4,l1Size-1);
			//SetEndPoint(tmpLi->At(lineId[0]),p);
			tmpLi->Add(l4);
			delete l1;
			delete l2;
			delete l3;

			TRACE("fork. don't add lines %i %i\n",lineId[1],lineId[2]);
		} else {
			TRACE("nofork\n");
		}
	}

	for(int i=0; i<tmpLi->Size(); i++) {
		if(!dontAddLine[i]) {
			newImage->Add(tmpLi->At(i)->Clone());
		} else {
			TRACE("didn't add %i\n",i);
		}
	}

	delete tmpLi;

	return newImage;
}

CFPoint CForkHandler::SmoothLine(CPolyLine* line, int index)
{
	CFPoint p = *(line->At(index-1)) * 0.5;
	p += *(line->At(index+1)) * 0.5;

	(*(line->At(index))).x = p.x;
	(*(line->At(index))).y = p.y;

	return p;
}

//when handling t forks: set connecting line endpoint to average of 
//the other spliced lines.
//this is not done very clean...
//a better idea is to extend the line until it collides with the
//other (new connected) line
void CForkHandler::SetEndPoint(CPolyLine* pl, CFPoint p)
{
	CFPoint *headPoint = pl->GetHeadPoint();
	CFPoint *tailPoint = pl->GetTailPoint();

	if(p.Distance(*headPoint) < p.Distance(*tailPoint)) {
		//modify headpoint
		pl->GetHeadPoint()->x = p.x;
		pl->GetHeadPoint()->y = p.y;
	} else {
		//modify tailpoint
		pl->GetTailPoint()->x = p.x;
		pl->GetTailPoint()->y = p.y;
	}
}

CLineImage* CForkHandler::HandleYForks(CLineImage* li)
{
	static const double SAME_LINE_MAX = -0.8;
	static const double SAME_LINE_TAIL_BOOST = 0.1;

	CLineImage *tmpLi = li->Clone();
	CLineImage *newImage = new CLineImage(li->GetWidth(),li->GetHeight());

	delete YForks;
	YForks = new vector<CFPoint>;

	map<int,bool> dontAddLine;
	vector<CFPoint>::iterator iter;

	for(iter=ThreeForks->begin(); iter!=ThreeForks->end(); iter++) {
		//iterate through all three-fork points

		vector<CFPoint> vectors;
		vector<int> lineId;

		TRACE("tmpLi->Size(): %i\n",tmpLi->Size());
		for(int i=0; i<tmpLi->Size(); i++) {
			if(dontAddLine[i]) continue;
			CPolyLine *line = tmpLi->At(i);

			assert(line->Size()>1);
			if(*(line->GetHeadPoint()) == *iter) {
				//fork point == line headpoint
				int j = min(2, line->Size()-1);
				CFPoint p = *line->At(j) - *line->At(j-1);
				vectors.push_back(p.Unit());
				lineId.push_back(i);
			} else if(*(line->GetTailPoint()) == *iter) {
				//fork point == line tailpoint
				int j = max(0, line->Size()-3);
				CFPoint p = *line->At(j) - *line->At(j+1);
				vectors.push_back(p.Unit());
				lineId.push_back(i);
			}
		}

		if(vectors.size()!=3) continue;
		TRACE("vectors.size(): %i\n",vectors.size());
		assert(vectors.size()==3);
		assert(lineId.size()==3);

		//calc cross products
		double C_01 = vectors[0] * vectors[1];
		double C_02 = vectors[0] * vectors[2];
		double C_12 = vectors[1] * vectors[2];

		//TRACE("C_01: %f\n",C_01);
		//TRACE("C_02: %f\n",C_02);
		//TRACE("C_12: %f\n",C_12);

		CPolyLine *line0 = tmpLi->At(lineId[0]);
		CPolyLine *line1 = tmpLi->At(lineId[1]);
		CPolyLine *line2 = tmpLi->At(lineId[2]);

		double sameLineMax0 = SAME_LINE_MAX;
		double sameLineMax1 = SAME_LINE_MAX;
		double sameLineMax2 = SAME_LINE_MAX;
		sameLineMax0 += line0->IsTail()?SAME_LINE_TAIL_BOOST:0;
		sameLineMax1 += line1->IsTail()?SAME_LINE_TAIL_BOOST:0;
		sameLineMax2 += line2->IsTail()?SAME_LINE_TAIL_BOOST:0;

		if(C_01 < sameLineMax0 && C_02 < sameLineMax0) {
			//attach baseline 0 to line 1,2
			YForks->push_back(*iter);
			dontAddLine[lineId[0]]=true;
			dontAddLine[lineId[1]]=true;
			dontAddLine[lineId[2]]=true;
			HandleYFork(tmpLi,line0,line1,line2,*iter);
		} else if(C_01 < sameLineMax1 && C_12 < sameLineMax1) {
			//attach baseline 1 to line 0,2
			YForks->push_back(*iter);
			dontAddLine[lineId[0]]=true;
			dontAddLine[lineId[1]]=true;
			dontAddLine[lineId[2]]=true;
			HandleYFork(tmpLi,line1,line0,line2,*iter);
		} else if(C_02 < sameLineMax2 && C_12 < sameLineMax2) {
			//attach baseline 2 to line 0,1
			YForks->push_back(*iter);
			dontAddLine[lineId[0]]=true;
			dontAddLine[lineId[1]]=true;
			dontAddLine[lineId[2]]=true;
			HandleYFork(tmpLi,line2,line1,line0,*iter);
		} else {
			TRACE("nofork\n");
		}
	}

	for(int i=0; i<tmpLi->Size(); i++) {
		if(!dontAddLine[i]) {
			newImage->Add(tmpLi->At(i)->Clone());
		} else {
			TRACE("didn't add %i\n",i);
		}
	}

	delete tmpLi;
	return newImage;
}

void CForkHandler::MarkYFork(CPolyLine* line, CFPoint p, int median)
{
	vector<CSketchPoint*>::iterator iter;
	vector<CSketchPoint*>::iterator iterEnd;

	if(line->GetHeadPoint()->Distance(p)<0.8) {
		//from head

		iter=line->Begin();
		(*iter)->m_yFork=true;
		iter++;
		iterEnd = line->End();
		iterEnd--;

		for(; iter!=iterEnd; iter++) {
			CFPoint *p = *iter;
			int dist=CBinarizer::Instance()->m_distanceMap->GetPixel(int(p->x+0.5),int(p->y+0.5));
			TRACE("dist: %i\n",dist);
			if(dist>median) {
				(*iter)->m_yFork=true;
			} else {
				break;
			}
		}
		
	} else {
		//from tail

		iterEnd = line->Begin();
		iterEnd++;

		iter = line->End();
		iter--;
		(*iter)->m_yFork=true;
		iter--;
		do {
			CFPoint *p = *iter;
			int dist=CBinarizer::Instance()->m_distanceMap->GetPixel(int(p->x+0.5),int(p->y+0.5));
			TRACE("dist: %i\n",dist);
			if(dist>median) {
				(*iter)->m_yFork=true;
			} else {
				break;
			}
			if(iter==iterEnd) break;
			iter--;
		} while(1);
	}
}

void CForkHandler::HandleYFork(CLineImage* img, CPolyLine* baseLine, CPolyLine* line1, CPolyLine* line2,CFPoint p)
{
	int median1 = line1->GetMedianThickness();
	int median2 = line2->GetMedianThickness();
	int median = baseLine->GetMedianThickness(); //min(median1,median2);

	MarkYFork(baseLine,p,median);
	MarkYFork(line1,p,median1);
	MarkYFork(line2,p,median2);

	CPolyLine *newBase = new CPolyLine();
	CPolyLine *newLine1= new CPolyLine();
	CPolyLine *newLine2 = new CPolyLine();

	//add new base line
	bool mergeBaseLine = baseLine->IsTail();
	TRACE("mergeBaseLine: %x\n",mergeBaseLine);
		//baseLine->GetHeadPoint()->Distance(*baseLine->GetTailPoint())<20 ? true : false;
	CSketchPoint *newEndPoint = 0;

	if(baseLine->GetTailPoint()->Distance(p)<0.8) {
		//add forwards
		int i=0;
		while(!baseLine->At(i)->m_yFork || baseLine->Size()-i>5) {
			newBase->Add(baseLine->At(i)->Clone());
			i++;
		}
		if(!mergeBaseLine && i!=0) {
			newEndPoint = baseLine->At(i-1);
		}
	} else {
		//add backwards
		int i=baseLine->Size()-1;
		while(!baseLine->At(i)->m_yFork || i>4) {
			newBase->Add(baseLine->At(i)->Clone());
			i--;
		}
		if(!mergeBaseLine && i!=baseLine->Size()-1) {
			newEndPoint = baseLine->At(i+1);
		}
	}


	//add line 1
	if(line1->GetTailPoint()->Distance(p)<0.8) {
		//add forwards
		int i=0;
		while(!line1->At(i)->m_yFork || (line1->Size()-i>5)) {
			newLine1->Add(line1->At(i)->Clone());
			i++;
		}
	} else {
		//add backwards
		int i=line1->Size()-1;
		while(!line1->At(i)->m_yFork || (i>4)) {
			newLine1->Add(line1->At(i)->Clone());
			i--;
		}
	}
	if(!mergeBaseLine) {
		//just connect to base line, but don't merge lines
		if(newEndPoint!=0) newLine1->Add(newEndPoint->Clone());
	} else {
		//merge lines
		for(int i=0; i<baseLine->Size(); i++) {
			newLine1->Add(baseLine->At(i)->Clone());
		}
	}

	//add line 2
	if(line2->GetTailPoint()->Distance(p)<0.8) {
		//add forwards
		int i=0;
		while(!line2->At(i)->m_yFork || ((line2->Size()-i)>5)) {
			newLine2->Add(line2->At(i)->Clone());
			i++;
		}
	} else {
		//add backwards
		int i=line2->Size()-1;
		while(!line2->At(i)->m_yFork || (i>4)) {
			newLine2->Add(line2->At(i)->Clone());
			i--;
		}
	}
	if(!mergeBaseLine) {
		//just connect to base line, but don't merge lines
		if(newEndPoint!=0) newLine2->Add(newEndPoint->Clone());
	} else {
		//merge lines
		for(int i=0; i<baseLine->Size(); i++) {
			newLine2->Add(baseLine->At(i)->Clone());
		}
	}

	if(!mergeBaseLine && newBase->Size()>1) {
		img->Add(newBase);
	} else {
		delete newBase;
		TRACE("do smooothpositions\n");
		CPolyLine *tmp = newLine1->SmoothPositions();
		delete newLine1;
		newLine1 = tmp;
		tmp = newLine2->SmoothPositions();
		delete newLine2;
		newLine2 = tmp;
	}
	img->Add(newLine1);
	img->Add(newLine2);
}
