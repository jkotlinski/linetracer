#include "StdAfx.h"
#include ".\beziermaker.h"

#include "PolyLine.h"
#include "LineImage.h"
#include "SketchPoint.h"

#include <assert.h>
#include <math.h>
#include <vector>

#define REUSE_TANGENTS

#define DO_BEZIER_TRACE

//#define MEM_DEBUG

CBezierMaker::CBezierMaker(void)
{
	SetParam("error_threshold",20);
}

CBezierMaker::~CBezierMaker(void)
{
}

CBezierMaker* CBezierMaker::Instance() {
    static CBezierMaker inst;
    return &inst;
}

CSketchImage* CBezierMaker::Process(CSketchImage* i_src)
{
	CLineImage *src = static_cast<CLineImage*>(i_src);


	//return ShaoZhou(src);
	CLineImage *dst = DoSchneider(src);
	dst->SolderKnots();
	return dst;
}

//simple method. just add good looking control points.
CSketchImage* CBezierMaker::SimpleMethod(CLineImage* src)
{
	CLineImage *dst = new CLineImage(src->GetWidth(),src->GetHeight());

	//make dst a copy of src
	for(int i=0; i<src->Size(); i++) {
		dst->Add(src->At(i)->Clone());
	}

	for(int i=0; i<dst->Size(); i++) {
		CPolyLine* pl = dst->At(i);
		vector<CSketchPoint*>::iterator point_iter=pl->Begin();

		vector<double> diffx;
		vector<double> diffy;
		vector<double> a;
		vector<CSketchPoint*> points;

		while(point_iter!=pl->End()) {
			points.push_back(*point_iter);
			point_iter++;
		}

		for(unsigned int i=0; i<points.size(); i++) {
			double xdiff; 
			double ydiff; 

			if(i==0) {
				xdiff = points.at(i+1)->x - points.at(i)->x;
				ydiff = points.at(i+1)->y - points.at(i)->y;
			} else {
				if(i==points.size()-1) {
					xdiff = points.at(i)->x - points.at(i-1)->x;
					ydiff = points.at(i)->y - points.at(i-1)->y;
				} else {
					xdiff = points.at(i+1)->x - points.at(i-1)->x;
					ydiff = points.at(i+1)->y - points.at(i-1)->y;
				}
			}
			double scale = sqrt(xdiff*xdiff+ydiff*ydiff);
			if(scale!=0) {
				xdiff/=scale;
				ydiff/=scale;
			} else {
				xdiff = ydiff = 0;
			}
			diffx.push_back(xdiff);
			diffy.push_back(ydiff);

			//TRACE("diff %i: %f %f; scale %f\n",i,xdiff,ydiff,scale);
		}

		for(unsigned int i=0; i<points.size()-1; i++) {
			double tmpx = points.at(i+1)->x - points.at(i)->x;
			double tmpy = points.at(i+1)->y - points.at(i)->y;
			double val = 0.4 * sqrt(tmpx*tmpx + tmpy*tmpy);
			a.push_back(val);
			//TRACE("a %i: %f\n",i,val);
		}

		for(unsigned int i=0; i<points.size()-1; i++) {
			//control point 1
			if(!points.at(i)->IsKnee()) {
				double x, y;
				x = a.at(i)*diffx.at(i) + points.at(i)->x;
				y = a.at(i)*diffy.at(i) + points.at(i)->y;
				pl->At(i)->SetControlPointForward(CFPoint(x,y));
			}

			//control point 2
			if(!points.at(i+1)->IsKnee()) {
				double x, y;
				x = -a.at(i)*diffx.at(i+1) + points.at(i+1)->x;
				y = -a.at(i)*diffy.at(i+1) + points.at(i+1)->y;
				pl->At(i+1)->SetControlPointBack(CFPoint(x,y));
			}
		}
	}
	return dst;
}

double CBezierMaker::FindError(CPolyLine* polyLine, CPolyLine* curve, vector<double> *tlist, int &worstPoint)
{
	worstPoint = -1;
	double totalError = 0;
	double worstError = -1;

	//TRACE("Curve->Size() %i\n",curve->Size());

	for(int i=0; i<polyLine->Size(); i++) {
		CFPoint linePoint = *(polyLine->At(i));
		double b = (*tlist)[i];

		CSketchPoint *start = curve->At(0);
		CSketchPoint *end = curve->At(1);

		//TRACE("b: %f\n",b);
		double t=1-b;

		//TRACE("t: %f\n",t);
		//TRACE("start: %f %f\n",start->x,start->y);
		//TRACE("start->cpf: %f %f\n",start->GetControlPointForward().x,start->GetControlPointForward().y);
		
		CFPoint curvePoint(
			start->x*t*t*t + start->GetControlPointForward().x*3*t*t*b + end->GetControlPointBack().x*3*t*b*b + end->x*b*b*b,
			start->y*t*t*t + start->GetControlPointForward().y*3*t*t*b + end->GetControlPointBack().y*3*t*b*b + end->y*b*b*b
			);

		CFPoint diff = curvePoint - linePoint;

		//TRACE("curvePoint: %f %f\n",curvePoint.x,curvePoint.y);
		//TRACE("linePoint: %f %f\n",linePoint.x,linePoint.y);
		//TRACE("diff: %f %f\n",diff.x,diff.y);

		double error = sqrt(diff.x*diff.x + diff.y*diff.y);

		//TRACE("error: %f\n",error);

		if(error > worstError) {
			worstPoint = i;
			worstError = error;
		}
		totalError += error;
		//TRACE("totalError: %f\n",totalError);
	}
	return totalError;
}

CLineImage* CBezierMaker::DoSchneider(CLineImage* src)
{
	static const double REPARAM_IMPROVEMENT = 0.1;
	static const double REPARAM_THRESHOLD = 30.0;

	double ERROR_THRESHOLD = GetParam("error_threshold");

	CLineImage *tmp = new CLineImage(src->GetWidth(),src->GetHeight());
	//make dst a copy of src
	for(int i=0; i<src->Size(); i++) {
		tmp->Add(src->At(i)->Clone());
	}

	CLineImage *dst = new CLineImage(src->GetWidth(),src->GetHeight());

#ifdef REUSE_TANGENTS
	map<int,bool> hasTangents;
	deque<vector<CFPoint>*> hasTangentList;
	//deque<vector<double>*> hasTList;
#endif

	//map<int,bool> hasTangents;
	//deque<vector<CFPoint>*> hasTangentList;

#ifdef MEM_DEBUG
	CMemoryState oldMemState, newMemState, diffMemState;
	oldMemState.Checkpoint();
#endif

	for(int i=0; i<tmp->Size(); i++) {
		CPolyLine* pl = tmp->At(i);

		//make tangent map
		vector<CFPoint>* tangentList;
		
#ifdef REUSE_TANGENTS
		if(hasTangents[i]) {
			tangentList = hasTangentList.front();
			hasTangentList.pop_front();

			//TRACE("popped tangentlist. new size: %i\n",hasTangentList.size());
			CSketchPoint startTangent = (*tangentList)[0];
			CSketchPoint endTangent = (*tangentList)[(*tangentList).size()-1];
			delete tangentList;

			tangentList = CalcTangents(pl,&startTangent,&endTangent);
			//tangentList = CalcTangents(pl);

			//tlist = hasTList.front();
			//hasTList.pop_front();
		} else {
			tangentList = CalcTangents(pl);
			//TRACE("calced tangentlist\n");
		}
#else
		tangentList = CalcTangents(pl);
#endif

		CPolyLine* curve = 0;
		bool doAddCurve = true;

		if(pl->Size()<3) {
			curve = FitLine(pl,tangentList);
		} else {
			//make T map
			vector<double>* tlist = CalcT(pl);
			/*TRACE("after calct:\n");
			for(unsigned int i=0; i<tlist->size(); i++) {
				TRACE("tlist[%i]: %f\n",i,tlist[i]);
			}*/

			double previousError = 1000000;
			double bestError = 1000000;

			CPolyLine * bestCurve = 0;
			int iteration = 0;
			int worstPoint = -1;
			while(true) {
				/*for(unsigned int i=0; i<tlist->size(); i++) {
					TRACE("tlist[%i]: %f\n",i,tlist[i]);
				}*/
				curve = FitSpline(pl,tlist,tangentList);
				double error = FindError(pl,curve,tlist,worstPoint);
				//TRACE("pl->size(): %i\n", pl->Size());
				//TRACE("curve->size(): %i\n", curve->Size());
				assert(error>=0);
				//TRACE("FindError: %f\n",error);
				if(error < previousError) {
					if(bestCurve != 0) {
						delete bestCurve;
					}
					bestError = error;
					bestCurve = curve->Clone();
					//TRACE("new bestError: %f\n",bestError);
				}

				//break;
				//TRACE("error: %f  preverror: %f\n",error,previousError);
				double improvement = 1;
				if(previousError!=0.0) {
					improvement -= error/previousError;
				} else {
					improvement = 0;
				}

				if(improvement<REPARAM_IMPROVEMENT || error > REPARAM_THRESHOLD) {
					break;
				}
				//TRACE("iteration: %i  improvement: %f\n",iteration++,improvement);

				if(!Reparametrize(pl,curve,tlist)) {
					break;
				}
				delete curve;

				previousError = error;
			}
			delete curve;

			curve = bestCurve;

			if(bestError > ERROR_THRESHOLD) {
				//subdivide polyline
				int subdivIndex = worstPoint;
				//TRACE("subdivide line\n");
				assert(worstPoint>=0);
				assert(worstPoint<pl->Size());
				CPolyLine * newBackLine = new CPolyLine();
				CPolyLine * newForwardLine = new CPolyLine();

#ifdef REUSE_TANGENTS
				vector<CFPoint>* backTangentList = new vector<CFPoint>;
				vector<CFPoint>* forwardTangentList = new vector<CFPoint>;
				//vector<double>* backTList = new vector<double>;
				//vector<double>* forwardTList = new vector<double>;
#endif
				//TRACE("backline add:\n");
				for(int j=0; j<=subdivIndex; j++) {
					newBackLine->Add(pl->At(j)->Clone());
					//TRACE("x y : %f %f\n",pl->At(j)->x,pl->At(j)->y);
#ifdef REUSE_TANGENTS
					backTangentList->push_back((*tangentList)[j]);
#endif
				}
				//TRACE("forwardline add:\n");
				for(int j=subdivIndex; j<pl->Size(); j++) {
					//TRACE("x y : %f %f\n",pl->At(j)->x,pl->At(j)->y);
					newForwardLine->Add(pl->At(j)->Clone());
#ifdef REUSE_TANGENTS
					forwardTangentList->push_back((*tangentList)[j]);
#endif
				}
				
				tmp->Add(newBackLine);
				tmp->Add(newForwardLine);

#ifdef REUSE_TANGENTS
				hasTangents[tmp->Size()-2]=true;
				hasTangents[tmp->Size()-1]=true;
				hasTangentList.push_back(backTangentList);
				hasTangentList.push_back(forwardTangentList);
				//hasTList.push_back(backTList);
				//hasTList.push_back(forwardTList);
				//TRACE("pushed tangentlists. new size: %i\n",hasTangentList.size());
				//delete backTangentList;
				//delete forwardTangentList;
#endif
				doAddCurve=false;
			}
			delete tlist;
		}
		delete tangentList;

		if(doAddCurve) {
			dst->Add(curve);
		} else {
			delete curve;
		}
	}

	delete tmp;

#ifdef MEM_DEBUG
	newMemState.Checkpoint();
	if( diffMemState.Difference( oldMemState, newMemState ) )
	{
		TRACE( "Memory leaked!\n" );
	}
#endif

	return dst;
}

vector<double>* CBezierMaker::CalcT(CPolyLine* line)
{
	vector<double>* tlist = new vector<double>;

	double totalDist = 0;

	//calculate total distance
	for(int i=1; i<line->Size(); i++) {
		double x1 = line->At(i-1)->x;
		double y1 = line->At(i-1)->y;
		double x2 = line->At(i)->x;
		double y2 = line->At(i)->y;
		double xdiff = x2-x1;
		double ydiff = y2-y1;
		totalDist += sqrt(xdiff*xdiff+ydiff*ydiff);
	}

	double currT = 0.0;
	//first point has T = 0.0
	tlist->push_back(0.0);

	for(int i=1; i<line->Size()-1; i++) {
		double x1 = line->At(i-1)->x;
		double y1 = line->At(i-1)->y;
		double x2 = line->At(i)->x;
		double y2 = line->At(i)->y;
		double xdiff = x2-x1;
		double ydiff = y2-y1;
		
		currT += sqrt(xdiff*xdiff+ydiff*ydiff)/totalDist;
		tlist->push_back(currT);
		//TRACE("T %i/%i: %f\n",i,line->Size()-1,currT);
	}

	tlist->push_back(1.0);

	return tlist;
}

vector<CFPoint>* CBezierMaker::CalcTangents(CPolyLine* line, CSketchPoint* startTangent, CSketchPoint* endTangent)
{
	vector<CFPoint>* tlist = new vector<CFPoint>;

	for(int i=0; i<line->Size(); i++) {

		double xdiff; 
		double ydiff;

		bool usingPrecalced = false;

		if(i==0) {
			//first point
			if(startTangent == NULL) {
				xdiff = line->At(i+1)->x - line->At(i)->x;
				ydiff = line->At(i+1)->y - line->At(i)->y;
			} else {
				xdiff = startTangent->x;
				ydiff = startTangent->y;
				//TRACE("use precalced start. x y: %f %f\n",xdiff,ydiff);
				usingPrecalced = true;
			}
		} else {
			if(i==line->Size()-1) {
				//endpoint
				if(endTangent == NULL) {
					xdiff = line->At(i)->x - line->At(i-1)->x;
					ydiff = line->At(i)->y - line->At(i-1)->y;
				} else {
					//use precalced tangent
					xdiff = endTangent->x;
					ydiff = endTangent->y;
					//TRACE("use precalced end. x y: %f %f\n",xdiff,ydiff);
					usingPrecalced = true;
				}
			} else {
				xdiff = line->At(i+1)->x - line->At(i-1)->x;
				ydiff = line->At(i+1)->y - line->At(i-1)->y;
			}
		}
		double scale = sqrt(xdiff*xdiff+ydiff*ydiff);
		if(scale!=0) {
			if(!usingPrecalced) {
				xdiff/=scale;
				ydiff/=scale;
				//TRACE("new tangent. x y: %f %f\n",xdiff,ydiff);
			}
		} else {
			xdiff = ydiff = 0;
		}
		tlist->push_back(CFPoint(xdiff,ydiff));
		//TRACE("i %i x %f y %f\n",i,xdiff,ydiff);
	}

	return tlist;
}

CPolyLine* CBezierMaker::FitSpline(CPolyLine* pl, vector<double>* tlist, vector<CFPoint>* tangentList)
{
	//TRACE("pl->Size(): %i\n",pl->Size());
	CFPoint startVector(pl->At(0)->x,pl->At(0)->y);
	CFPoint endVector(pl->At(pl->Size()-1)->x,pl->At(pl->Size()-1)->y);

	//TRACE("startVector: %f %f\n",startVector.x,startVector.y);
	//TRACE("endVector: %f %f\n",endVector.x,endVector.y);

	CPolyLine *curve = new CPolyLine();
	curve->Add(new CSketchPoint(startVector.x,startVector.y,true));
	curve->Add(new CSketchPoint(endVector.x,endVector.y,true));

	double X_C1_det,C0_X_det,C0_C1_det;
	double alpha1,alpha2;
	double C[2][2]={{0.0,0.0},{0.0,0.0}};
	double X[2]={0.0,0.0};

	vector<CFPoint> A1v;
	vector<CFPoint> A2v;

	CFPoint t1_hat = tangentList->at(0);
	CFPoint t2_hat = -tangentList->at(tangentList->size()-1);
	//TRACE("t1_hat: %f,%f\n",t1_hat.x,t1_hat.y);
	//TRACE("t2_hat: %f,%f\n",t2_hat.x,t2_hat.y);

	for(int i=0; i<pl->Size(); i++) {
		CFPoint A1 = t1_hat * (3 * (*tlist)[i] * (1-(*tlist)[i]) * (1-(*tlist)[i]));
		CFPoint A2 = t2_hat * (3 * (*tlist)[i] * (*tlist)[i] * (1-(*tlist)[i]));
		A1v.push_back(A1);
		A2v.push_back(A2);
		//TRACE("%i A1: %f %f; A2: %f %f\n",i,A1.x,A1.y,A2.x,A2.y);
	}
	for(int i=0; i<pl->Size(); i++) {
		//TRACE("A1v[%i]: %f %f\n",i,A1v[i].x,A1v[i].y);
		//TRACE("A2v[%i]: %f %f\n",i,A2v[i].x,A2v[i].y);
		//TRACE("(*tlist)[%i]: %f\n",i,(*tlist)[i]);

		C[0][0] += A1v[i] * A1v[i];
		C[1][0] += A1v[i] * A2v[i];
		//C[0][1] += A1v[i] * A2v[i]; do outside loop
		C[1][1] += A2v[i] * A2v[i];

		CFPoint temp0 = startVector * (1-(*tlist)[i]) * (1-(*tlist)[i]) * (1-(*tlist)[i]);
		CFPoint temp1 = startVector * 3 * (*tlist)[i] * (1-(*tlist)[i]) * (1-(*tlist)[i]);
		CFPoint temp2 = endVector * 3 * (*tlist)[i] * (*tlist)[i] * (1-(*tlist)[i]);
		CFPoint temp3 = endVector * (*tlist)[i] * (*tlist)[i] * (*tlist)[i];

		CFPoint temp = (*pl->At(i)) - (temp0+temp1+temp2+temp3);
		//TRACE("%i temp: %f %f\n",i,temp.x,temp.y);
		X[0] += temp * A1v[i];
		X[1] += temp * A2v[i];
	}
	C[0][1] = C[1][0];

	//TRACE("C[0][0] %f\n",C[0][0]);
	//TRACE("C[0][1] %f\n",C[0][1]);
	//TRACE("C[1][1] %f\n",C[1][1]);

	X_C1_det = X[0] * C[1][1] - X[1] * C[0][1];
	C0_X_det = C[0][0] * X[1] - C[0][1] * X[0];
	C0_C1_det = C[0][0] * C[1][1] - C[0][1] * C[1][0];

	//TRACE("X_C1_det: %f\n",X_C1_det);
	//TRACE("C0_X_det: %f\n",C0_X_det);
	//TRACE("C0_C1_det: %f\n",C0_C1_det);

	//
	//assert(C0_C1_det!=0);
	if(C0_C1_det == 0) {
		//TRACE("ERROR!!! do line\n");
		curve->At(0)->SetControlPointForward(startVector);
		curve->At(1)->SetControlPointBack(startVector);
	} else {
		alpha1 = X_C1_det / C0_C1_det;
		alpha2 = C0_X_det / C0_C1_det;

		//TRACE("alpha1: %f\n",alpha1);
		//TRACE("alpha2: %f\n",alpha2);
		curve->At(0)->SetControlPointForward(startVector + t1_hat * alpha1);
		curve->At(1)->SetControlPointBack(endVector + t2_hat * alpha2);
	}

	return curve;
}

CPolyLine* CBezierMaker::FitLine(CPolyLine* pl, vector<CFPoint>* tangentList)
{
	CPolyLine *curve = new CPolyLine();

	CFPoint start = *(pl->At(0));
	CFPoint end = *(pl->At(1));
	//CFPoint diff = end-start;
	//double dist = sqrt(diff.x*diff.x+diff.y*diff.y);

	//CFPoint cp1 = start + (*tangentList)[0] * dist * 0.1;
	//CFPoint cp2 = end - (*tangentList)[1] * dist * 0.1;

	CSketchPoint *startPoint = new CSketchPoint(start.x,start.y,true);
	CSketchPoint *endPoint = new CSketchPoint(end.x,end.y,true);

	//startPoint->SetControlPointForward(cp1);
	//endPoint->SetControlPointBack(cp2);

	curve->Add(startPoint);
	curve->Add(endPoint);

	return curve;
}

bool CBezierMaker::Reparametrize(CPolyLine* pl, CPolyLine* curve,vector<double>* tlist)
{
	CFPoint curveD1[3];
	CFPoint curveD2[2];
	
	//TRACE("curve in : %f %f; %f %f; %f %f; %f %f\n",curve->At(0)->x,curve->At(0)->y,curve->At(0)->GetControlPointForward().x,curve->At(0)->GetControlPointForward().y,curve->At(1)->GetControlPointBack().x,curve->At(1)->GetControlPointBack().y,curve->At(1)->x,curve->At(1)->y);

	//1st derivate of curve
	curveD1[0] = curve->At(0)->GetControlPointForward() - *(curve->At(0));
	curveD1[1] = curve->At(1)->GetControlPointBack() - curve->At(0)->GetControlPointForward();
	curveD1[2] = *(curve->At(1)) - curve->At(1)->GetControlPointBack();

	//TRACE("curve' in : %f %f; %f %f; %f %f\n",curveD1[0].x,curveD1[0].y,curveD1[1].x,curveD1[1].y,curveD1[2].x,curveD1[2].y);

	//2nd derivate of curve
	curveD2[0] = curveD1[1] - curveD1[0];
	curveD2[1] = curveD1[2] - curveD1[1];

	//TRACE("curve'' in : %f %f; %f %f\n",curveD2[0].x,curveD2[0].y,curveD2[1].x,curveD2[1].y);

	for(int i=0; i<pl->Size(); i++) {
		double b = (*tlist)[i];
		double a = 1 - b;

		CFPoint curvePoint0;
		CFPoint curvePoint1;
		CFPoint curvePoint2;

		curvePoint0.x = curve->At(0)->x*a*a*a + curve->At(0)->GetControlPointForward().x*3*a*a*b + curve->At(1)->GetControlPointBack().x*3*a*b*b + curve->At(1)->x*b*b*b;
		curvePoint0.y = curve->At(0)->y*a*a*a + curve->At(0)->GetControlPointForward().y*3*a*a*b + curve->At(1)->GetControlPointBack().y*3*a*b*b + curve->At(1)->y*b*b*b;

		curvePoint1.x = curveD1[0].x*a*a + curveD1[1].x*a*b + curveD1[2].x*b*b;
		curvePoint1.y = curveD1[0].y*a*a + curveD1[1].y*a*b + curveD1[2].y*b*b;

		curvePoint2.x = curveD2[0].x*a + curveD2[1].x*b;
		curvePoint2.y = curveD2[0].y*a + curveD2[1].y*b;

		CFPoint diff = curvePoint0 - *(pl->At(i));

		// f(t)/f'(t)
		double numerator = diff.x * curvePoint1.x + diff.y * curvePoint1.y;
		double denominator = curvePoint1.x * curvePoint1.x + diff.x * curvePoint2.x;
		denominator += curvePoint1.y * curvePoint1.y + diff.y * curvePoint2.y;

		//calculate distances
		double oldDistance = sqrt(diff.x*diff.x+diff.y*diff.y);
		(*tlist)[i] -= numerator/denominator;
		b = (*tlist)[i];
		a = 1-b;
		CSketchPoint *start = curve->At(0);
		CSketchPoint *end = curve->At(1);
		CFPoint newPoint(
			start->x*a*a*a + start->GetControlPointForward().x*3*a*a*b + end->GetControlPointBack().x*3*a*b*b + end->x*b*b*b,
			start->y*a*a*a + start->GetControlPointForward().y*3*a*a*b + end->GetControlPointBack().y*3*a*b*b + end->y*b*b*b
			);
		diff = newPoint - *(pl->At(i));
		double newDistance = sqrt(diff.x*diff.x+diff.y*diff.y);

		//TRACE("oldDistance: %f\n",oldDistance);
		//TRACE("newDistance: %f\n",newDistance);
		if(newDistance > oldDistance) {
			//TRACE("newDistance > oldDistance!\n");
			return false;
		}

	}

	//TRACE("SUCCESS\n");
	return true;
}
