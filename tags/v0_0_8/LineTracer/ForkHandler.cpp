#include "StdAfx.h"
#include "forkhandler.h"

#include "Logger.h"
#include "FPoint.h"
#include "PolyLine.h"

#include <math.h>
#include <vector>
#include <map>

#include "Binarizer.h"

CForkHandler::CForkHandler(void)
: CImageProcessor()
, ThreeForks(NULL)
, TForks(NULL)
, YForks(NULL)
{
	SetName(CString("forkhandler"));
	ThreeForks = new vector<CFPoint>;
	TForks = new vector<CFPoint>;
	YForks = new vector<CFPoint>;
}

CForkHandler::~CForkHandler(void)
{
	try {
		delete ThreeForks;
		delete TForks;
		delete YForks;
	} 
	catch (...) {
		try {
			LOG ( "!!! Warning! caught exception in ~CForkHandler" );
		}
		catch (...) {
		}
	}
}

CForkHandler* CForkHandler::Instance() {
    static CForkHandler inst;
    return &inst;
}

CSketchImage* CForkHandler::Process(CSketchImage* i_src)
{
	CLineImage *src = dynamic_cast<CLineImage*>(i_src);
	ASSERT ( src != NULL );

	delete ThreeForks;
	ThreeForks = Find3Forks(src);

	CLineImage *tmp = HandleTForks(src);
	//causes memory leak!!!
	CLineImage *tmp2 = HandleYForks(tmp);
	
	delete tmp;
	return tmp2;
}

vector<CFPoint>* CForkHandler::Find3Forks(const CLineImage *li) const
{
	ASSERT ( li != NULL );
	CLogger::Inactivate();

	map<CFPoint,int> counter;
	vector<CFPoint>* forks = new vector<CFPoint>;

	for(unsigned int i=0; i<li->Size(); i++) {
		CPolyLine *line = li->GetLine(i);

		CFPoint l_headCoords = line->GetHeadPoint()->GetCoords();
		CFPoint l_tailCoords = line->GetTailPoint()->GetCoords();

		if( l_headCoords != l_tailCoords ) {
			counter[l_headCoords] = counter[l_headCoords] + 1;
			counter[l_tailCoords] = counter[l_tailCoords] + 1;
		} else {
			counter[l_headCoords] = -10000;
			LOG("WARNING: line headpoint equals tailpoint!\n");
		}
	}

	map<CFPoint,int>::iterator iter;

	for(iter=counter.begin(); iter!=counter.end(); ++iter) {
		if((*iter).second==3) {
			forks->push_back((*iter).first);
			LOG("threefork: %f %f\n",(*iter).first.GetX(),(*iter).first.GetY());
		}
	}
	return forks;
}

CLineImage* CForkHandler::HandleTForks(const CLineImage* li)
{
	CLogger::Inactivate();

	static const double SAME_LINE_MAX = -0.8;
	static const double CROSS_LINE_MIN = -0.75;

	CLineImage *tmpLi = li->Clone();
	CLineImage *newImage = new CLineImage(li->GetWidth(),li->GetHeight());

	delete TForks;
	TForks = new vector<CFPoint>;

	vector<CFPoint>::iterator iter;
	map<unsigned int,bool> dontAddLine;

	for(iter=ThreeForks->begin(); iter!=ThreeForks->end(); ++iter) {
		//iterate through all three-fork points

		vector<CFPoint> vectors;
		vector<int> lineId;

		LOG("tmpLi->Size(): %i\n",tmpLi->Size());
		for(unsigned int i=0; i<tmpLi->Size(); i++) {
			if(dontAddLine[i]) continue;
			CPolyLine *line = tmpLi->GetLine(i);

			if( line->GetHeadPoint()->GetCoords() == *iter) 
			{
				//fork point == line headpoint
				unsigned int j = line->Size()-1;
				j = min(2, j);

				// check for wrapping error
				ASSERT ( j < 1000000 );

				CFPoint p = line->At(j)->GetCoords() - line->At(j-1)->GetCoords();
				vectors.push_back(p.Unit());
				lineId.push_back(i);
			} 
			else if(line->GetTailPoint()->GetCoords() == *iter) {
				//fork point == line tailpoint
				int j = line->Size()-3;
				j = max ( 0, j );

				CFPoint p = line->At(j)->GetCoords() - line->At(j+1)->GetCoords();
				vectors.push_back(p.Unit());
				lineId.push_back(i);
			}
		}

		if(vectors.size()!=3) continue;
		LOG("vectors.size(): %i\n",vectors.size());
		ASSERT(vectors.size()==3);
		ASSERT(lineId.size()==3);

		//calc cross products
		double C_01 = vectors[0] * vectors[1];
		double C_02 = vectors[0] * vectors[2];
		double C_12 = vectors[1] * vectors[2];

		//LOG("C_01: %f\n",C_01);
		//LOG("C_02: %f\n",C_02);
		//LOG("C_12: %f\n",C_12);

		//doesn't do anything -- remove?
		//tmpLi->IsTail(tmpLi->At(lineId[0]));

		if(C_01 < SAME_LINE_MAX && C_02 > CROSS_LINE_MIN && C_12 > CROSS_LINE_MIN) {
			//merge line 0 and 1
			TForks->push_back(*iter);
			dontAddLine[lineId[0]]=true;
			dontAddLine[lineId[1]]=true;
			CPolyLine *l1 = tmpLi->GetLine(lineId[0])->Clone();
			CPolyLine *l2 = tmpLi->GetLine(lineId[1])->Clone();
			CPolyLine *l3 = l1->MergeLine(l2);
			CPolyLine *l4 = l3->Clone();
			int l1Size = l1->Size();
			int l_connectorPointIndex = l1Size-1;
			l4->SmoothPoint(l_connectorPointIndex);
			//SetEndPoint(tmpLi->At(lineId[2]),p);
			tmpLi->Add(l4);
			delete l1;
			delete l2;
			delete l3;
			//tmpLi->Add(l2);
			LOG("fork. don't add lines %i %i\n",lineId[0],lineId[1]);
		} else if(C_02 < SAME_LINE_MAX && C_01 > CROSS_LINE_MIN && C_12 > CROSS_LINE_MIN) {
			//merge line 0 and 2
			TForks->push_back(*iter);
			dontAddLine[lineId[0]]=true;
			dontAddLine[lineId[2]]=true;
			CPolyLine *l1 = tmpLi->GetLine(lineId[0])->Clone();
			CPolyLine *l2 = tmpLi->GetLine(lineId[2])->Clone();
			CPolyLine *l3 = l1->MergeLine(l2);
			CPolyLine *l4 = l3->Clone();
			int l1Size = l1->Size();
			int l_connectorPointIndex = l1Size-1;
			l4->SmoothPoint(l_connectorPointIndex);
			//SetEndPoint(tmpLi->At(lineId[1]),p);
			tmpLi->Add(l4);
			delete l1;
			delete l2;
			delete l3;
			LOG("fork. don't add lines %i %i\n",lineId[0],lineId[2]);
		} else if(C_12 < SAME_LINE_MAX && C_01 > CROSS_LINE_MIN && C_02 > CROSS_LINE_MIN) {
			//merge line 1 and 2
			TForks->push_back(*iter);
			dontAddLine[lineId[1]]=true;
			dontAddLine[lineId[2]]=true;
			CPolyLine *l1 = tmpLi->GetLine(lineId[1])->Clone();
			CPolyLine *l2 = tmpLi->GetLine(lineId[2])->Clone();
			CPolyLine *l3 = l1->MergeLine(l2);
			CPolyLine *l4 = l3->Clone();
			int l1Size = l1->Size();
			int l_connectorPointIndex = l1Size-1;
			l4->SmoothPoint(l_connectorPointIndex);
			//SetEndPoint(tmpLi->At(lineId[0]),p);
			tmpLi->Add(l4);
			delete l1;
			delete l2;
			delete l3;

			LOG("fork. don't add lines %i %i\n",lineId[1],lineId[2]);
		} else {
			LOG("nofork\n");
		}
	}

	for(unsigned int i=0; i<tmpLi->Size(); i++) {
		if(!dontAddLine[i]) {
			newImage->Add(tmpLi->GetLine(i)->Clone());
		} else {
			LOG("didn't add %i\n",i);
		}
	}

	delete tmpLi;

	return newImage;
}

//when handling t forks: set connecting line endpoint to average of 
//the other spliced lines.
//this is not done very clean...
//a better idea is to extend the line until it collides with the
//other (new connected) line
void CForkHandler::SetEndPoint(CPolyLine* pl, const CFPoint &p) const
{
	CFPoint headPoint = pl->GetHeadPoint()->GetCoords();
	CFPoint tailPoint = pl->GetTailPoint()->GetCoords();

	if(p.Distance(headPoint) < p.Distance(tailPoint)) {
		//modify headpoint
		pl->GetHeadPoint()->SetX( p.GetX() );
		pl->GetHeadPoint()->SetY( p.GetY() );
	} else {
		//modify tailpoint
		pl->GetTailPoint()->SetX ( p.GetX() );
		pl->GetTailPoint()->SetY ( p.GetY() );
	}
}

CLineImage* CForkHandler::HandleYForks(const CLineImage* li)
{
	CLogger::Instance()->Inactivate();

	static const double SAME_LINE_MAX = -0.8;
	static const double SAME_LINE_TAIL_BOOST = 0.1;

	CLineImage *tmpLi = li->Clone();
	CLineImage *newImage = new CLineImage(li->GetWidth(),li->GetHeight());

	delete YForks;
	YForks = new vector<CFPoint>;

	map<int,bool> dontAddLine;
	vector<CFPoint>::iterator iter;

	for(iter=ThreeForks->begin(); iter!=ThreeForks->end(); ++iter) {
		//iterate through all three-fork points

		vector<CFPoint> vectors;
		vector<int> lineId;

		LOG("tmpLi->Size(): %i\n",tmpLi->Size());
		for(unsigned int i=0; i<tmpLi->Size(); i++) {
			if(dontAddLine[i]) continue;
			CPolyLine *line = tmpLi->GetLine(i);

			ASSERT(line->Size() > 1);
			if(line->GetHeadPoint()->GetCoords() == *iter) {
				//fork point == line headpoint
				int j = line->Size() - 1;
				j = min (j, 2);
				CFPoint p = line->At(j)->GetCoords() - line->At(j-1)->GetCoords();
				vectors.push_back(p.Unit());
				lineId.push_back(i);
			} else if(line->GetTailPoint()->GetCoords() == *iter) {
				//fork point == line tailpoint
				int j = line->Size() - 3;
				j = max(0, j);

				CFPoint p = line->At(j)->GetCoords() - line->At(j+1)->GetCoords();
				vectors.push_back(p.Unit());
				lineId.push_back(i);
			}
		}

		if(vectors.size()!=3) continue;
		LOG("vectors.size(): %i\n",vectors.size());
		ASSERT(vectors.size()==3);
		ASSERT(lineId.size()==3);

		//calc cross products
		double C_01 = vectors[0] * vectors[1];
		double C_02 = vectors[0] * vectors[2];
		double C_12 = vectors[1] * vectors[2];

		//LOG("C_01: %f\n",C_01);
		//LOG("C_02: %f\n",C_02);
		//LOG("C_12: %f\n",C_12);

		CPolyLine *line0 = tmpLi->GetLine(lineId[0]);
		CPolyLine *line1 = tmpLi->GetLine(lineId[1]);
		CPolyLine *line2 = tmpLi->GetLine(lineId[2]);

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
			LOG("nofork\n");
		}
	}

	for(unsigned int i=0; i<tmpLi->Size(); i++) {
		if(!dontAddLine[i]) {
			newImage->Add(tmpLi->GetLine(i)->Clone());
		} else {
			LOG("didn't add %i\n",i);
		}
	}

	delete tmpLi;
	return newImage;
}

void CForkHandler::MarkYFork(CPolyLine* line, const CFPoint &p, int median) const
{
	CLogger::Instance()->Inactivate();

	vector<CSketchPoint*>::iterator iter;
	vector<CSketchPoint*>::iterator iterEnd;

	if(line->GetHeadPoint()->Distance(p)<0.8) {
		//from head

		iter=line->Begin();
		(*iter)->SetIsYFork( true );
		++iter;
		iterEnd = line->End();
		--iterEnd;

		for(; iter!=iterEnd; ++iter) {
			CFPoint l_p = (*iter)->GetCoords();
			int dist=CBinarizer::Instance()->GetDistanceMap()->GetPixel(int(l_p.GetX()+0.5),int(l_p.GetY()+0.5));
			LOG("dist: %i\n",dist);
			if(dist>median) {
				(*iter)->SetIsYFork(true);
			} else {
				break;
			}
		}
		
	} else {
		//from tail

		iterEnd = line->Begin();
		++iterEnd;

		iter = line->End();
		--iter;
		(*iter)->SetIsYFork( true );
		--iter;
		for(;;) {
			CFPoint l_p = (*iter)->GetCoords();
			int dist=CBinarizer::Instance()->GetDistanceMap()->GetPixel(int(l_p.GetX()+0.5),int(l_p.GetY()+0.5));
			LOG("dist: %i\n",dist);
			if(dist>median) {
				(*iter)->SetIsYFork( true );
			} else {
				break;
			}
			if(iter==iterEnd) break;
			--iter;
		}
	}
}

void CForkHandler::HandleYFork(CLineImage* img, CPolyLine* baseLine, CPolyLine* line1, CPolyLine* line2, const CFPoint &p)
{
	CLogger::Inactivate();

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
	LOG("mergeBaseLine: %x\n", mergeBaseLine?1:0 );
		//baseLine->GetHeadPoint()->Distance(*baseLine->GetTailPoint())<20 ? true : false;
	CSketchPoint *newEndPoint = 0;

	if(baseLine->GetTailPoint()->Distance(p)<0.8) {
		//add forwards
		int i=0;
		while(!baseLine->At(i)->IsYFork() || baseLine->Size()-i>5) {
			newBase->Add(baseLine->At(i)->Clone());
			i++;
		}
		if(!mergeBaseLine && i!=0) {
			newEndPoint = baseLine->At(i-1);
		}
	} else {
		//add backwards
		int i=baseLine->Size()-1;
		while(!baseLine->At(i)->IsYFork() || i>4) {
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
		while(!line1->At(i)->IsYFork() || (line1->Size()-i>5)) {
			newLine1->Add(line1->At(i)->Clone());
			i++;
		}
	} else {
		//add backwards
		int i=line1->Size()-1;
		while(!line1->At(i)->IsYFork() || (i>4)) {
			newLine1->Add(line1->At(i)->Clone());
			i--;
		}
	}
	if(!mergeBaseLine) {
		//just connect to base line, but don't merge lines
		if(newEndPoint!=0) newLine1->Add(newEndPoint->Clone());
	} else {
		//merge lines
		for(unsigned int i=0; i<baseLine->Size(); i++) {
			newLine1->Add(baseLine->At(i)->Clone());
		}
	}

	//add line 2
	if(line2->GetTailPoint()->Distance(p)<0.8) {
		//add forwards
		int i=0;
		while(!line2->At(i)->IsYFork() || ((line2->Size()-i)>5)) {
			newLine2->Add(line2->At(i)->Clone());
			i++;
		}
	} else {
		//add backwards
		int i=line2->Size()-1;
		while(!line2->At(i)->IsYFork() || (i>4)) {
			newLine2->Add(line2->At(i)->Clone());
			i--;
		}
	}
	if(!mergeBaseLine) {
		//just connect to base line, but don't merge lines
		if(newEndPoint!=0) newLine2->Add(newEndPoint->Clone());
	} else {
		//merge lines
		for(unsigned int i=0; i<baseLine->Size(); i++) {
			newLine2->Add(baseLine->At(i)->Clone());
		}
	}

	if(!mergeBaseLine && newBase->Size()>1) {
		img->Add(newBase);
	} else {
		delete newBase;
		LOG("do smooothpositions\n");
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
