#include "StdAfx.h"
#include "beziermaker.h"

#include "Logger.h"
#include "PolyLine.h"
#include "LineImage.h"
#include "SketchPoint.h"
#include "LineImagePainter.h"

#include <math.h>
#include <vector>

//#define MEM_DEBUG

CBezierMaker::CBezierMaker(void)
: CImageProcessor()
{
	SetParam(BEZIERMAKER_ERROR_THRESHOLD, 20.0);
	SetName( new CString("Bezier Maker") );
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
	CLineImage *src = dynamic_cast<CLineImage*>(i_src);
	ASSERT ( src != NULL );

	//return ShaoZhou(src);
	CLineImage *dst = DoSchneider(src);
	dst->SolderKnots();
	return dst;
}

//simple method. just add good looking control points.
CSketchImage* CBezierMaker::SimpleMethod(const CLineImage* src) const
{
	CLineImage *dst = new CLineImage(src->GetWidth(),src->GetHeight());

	//make dst a copy of src
	for(unsigned int l_lineIndex=0; l_lineIndex<src->Size(); l_lineIndex++) {
		dst->Add(src->At(l_lineIndex)->Clone());
	}

	for(unsigned int i=0; i<dst->Size(); i++) {
		CPolyLine* pl = dst->At(i);
		vector<CSketchPoint*>::iterator point_iter=pl->Begin();

		vector<double> diffx;
		vector<double> diffy;
		vector<double> a;
		vector<CSketchPoint*> points;

		while(point_iter!=pl->End()) {
			points.push_back(*point_iter);
			++point_iter;
		}

		for(unsigned int l_pointIndex=0; l_pointIndex<points.size(); l_pointIndex++) {
			double xdiff; 
			double ydiff; 

			if(l_pointIndex==0) {
				xdiff = points.at(l_pointIndex+1)->GetX() - points.at(l_pointIndex)->GetX();
				ydiff = points.at(l_pointIndex+1)->GetY() - points.at(l_pointIndex)->GetY();
			} else {
				if(l_pointIndex==points.size()-1) {
					xdiff = points.at(l_pointIndex)->GetX() - points.at(l_pointIndex-1)->GetX();
					ydiff = points.at(l_pointIndex)->GetY() - points.at(l_pointIndex-1)->GetY();
				} else {
					xdiff = points.at(l_pointIndex+1)->GetX() - points.at(l_pointIndex-1)->GetX();
					ydiff = points.at(l_pointIndex+1)->GetY() - points.at(l_pointIndex-1)->GetY();
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

			//LOG("diff %i: %f %f; scale %f\n",i,xdiff,ydiff,scale);
		}

		for(unsigned int l_point=0; l_point<points.size()-1; l_point++) {
			double tmpx = points.at(l_point+1)->GetX() - points.at(l_point)->GetX();
			double tmpy = points.at(l_point+1)->GetY() - points.at(l_point)->GetY();
			double val = 0.4 * sqrt(tmpx*tmpx + tmpy*tmpy);
			a.push_back(val);
		}

		for(unsigned int l_point2=0; l_point2 < points.size()-1; l_point2++) {
			//control point 1
			if(!points.at(l_point2)->IsKnee()) {
				double x, y;
				x = a.at(l_point2)*diffx.at(l_point2) + points.at(l_point2)->GetX();
				y = a.at(l_point2)*diffy.at(l_point2) + points.at(l_point2)->GetY();
				pl->At(l_point2)->SetControlPointForward(CFPoint(x,y));
			}

			//control point 2
			if(!points.at(l_point2+1)->IsKnee()) {
				double x, y;
				x = -a.at(l_point2)*diffx.at(l_point2+1) + points.at(l_point2+1)->GetX();
				y = -a.at(l_point2)*diffy.at(l_point2+1) + points.at(l_point2+1)->GetY();
				pl->At(l_point2+1)->SetControlPointBack(CFPoint(x,y));
			}
		}
	}
	return dst;
}

double CBezierMaker::FindError(CPolyLine* polyLine, CPolyLine* curve, vector<double> *tlist, unsigned int &worstPoint) const
{
	worstPoint = 0xffffffff;
	double totalError = 0;
	double worstError = -1;

	//LOG("Curve->Size() %i\n",curve->Size());

	for(unsigned int i=0; i<polyLine->Size(); i++) {
		CFPoint linePoint = polyLine->At(i)->GetCoords();
		double b = (*tlist)[i];

		CSketchPoint *start = curve->At(0);
		CSketchPoint *end = curve->At(1);

		//LOG("b: %f\n",b);
		double t=1-b;

		//LOG("t: %f\n",t);
		//LOG("start: %f %f\n",start->GetX(),start->GetY());
		//LOG("start->cpf: %f %f\n",start->GetControlPointForward().GetX(),start->GetControlPointForward().GetY());
		
		CFPoint curvePoint(
			start->GetX()*t*t*t + start->GetControlPointForward().GetX()*3*t*t*b + end->GetControlPointBack().GetX()*3*t*b*b + end->GetX()*b*b*b,
			start->GetY()*t*t*t + start->GetControlPointForward().GetY()*3*t*t*b + end->GetControlPointBack().GetY()*3*t*b*b + end->GetY()*b*b*b
			);

		CFPoint diff = curvePoint - linePoint;

		//LOG("curvePoint: %f %f\n",curvePoint.GetX(),curvePoint.GetY());
		//LOG("linePoint: %f %f\n",linePoint.GetX(),linePoint.GetY());
		//LOG("diff: %f %f\n",diff.GetX(),diff.GetY());

		double error = sqrt(diff.GetX()*diff.GetX() + diff.GetY()*diff.GetY());

		//LOG("error: %f\n",error);

		if(error > worstError) {
			worstPoint = i;
			worstError = error;
		}
		totalError += error;
		//LOG("totalError: %f\n",totalError);
	}
	ASSERT ( worstPoint != 0xffffffff );
	return totalError;
}

CLineImage* CBezierMaker::DoSchneider(const CLineImage* src)
{
	ASSERT ( src != NULL );
#define REUSE_TANGENTS

	static const double REPARAM_THRESHOLD = 30.0;

	double ERROR_THRESHOLD = GetParam(BEZIERMAKER_ERROR_THRESHOLD);

	//LOG( "DoSchneider()\n" );

	CLineImage *l_workImage = new CLineImage(src->GetWidth(),src->GetHeight());
	//make dst a copy of src
	for(unsigned int i=0; i<src->Size(); i++) {
		l_workImage->Add(src->At(i)->Clone());
	}

	CLineImage *dst = new CLineImage(src->GetWidth(),src->GetHeight());

#ifdef REUSE_TANGENTS
	map<unsigned int,bool> hasTangents;
	deque<vector<CFPoint>*> hasTangentList;
#endif

#ifdef MEM_DEBUG
	CMemoryState oldMemState, newMemState, diffMemState;
	oldMemState.Checkpoint();
#endif

	for(unsigned int l_lineIndex=0; l_lineIndex<l_workImage->Size(); l_lineIndex++) {
		CPolyLine* pl = l_workImage->At(l_lineIndex);

		ASSERT ( 0 == pl->RemoveDuplicatePoints() );

		//make tangent map
		vector<CFPoint>* tangentList;
		
#ifdef REUSE_TANGENTS
		if(hasTangents[l_lineIndex]) {
			tangentList = hasTangentList.front();
			hasTangentList.pop_front();

			//LOG("popped tangentlist. new size: %i\n",hasTangentList.size());
			CSketchPoint startTangent ( (*tangentList)[0] );
			CSketchPoint endTangent ( (*tangentList)[(*tangentList).size()-1] );
			delete tangentList;

			tangentList = CalcTangents(pl,&startTangent,&endTangent);
		} else {
			tangentList = CalcTangents(pl);
		}
#else
		tangentList = CalcTangents(pl);
#endif

		CPolyLine* curve = 0;
		bool doAddCurve = true;

		if(pl->Size()<3) 
		{
			// straight line
			curve = FitLine(pl,tangentList);
		} 
		else 
		{
			// calculate T list
			vector<double>* tlist = CalcT(pl);

			static const double L_VERY_BIG_NUMBER = 100000000.0;

			double previousError = L_VERY_BIG_NUMBER;
			double bestError = L_VERY_BIG_NUMBER;

			CPolyLine * bestCurve = NULL;
			unsigned int worstPoint = 0xffffffff;

			//forever loop with two exits
			for ( ;; )
			{
				curve = FitSpline(pl, tlist, tangentList);

				double error = FindError(pl, curve, tlist, worstPoint);

				ASSERT(error >= 0);

				bool l_errorTooBig = error > REPARAM_THRESHOLD;
				if ( l_errorTooBig ) 
				{
					//give up curvefitting - we'll subdivide line and try again
					break;
				}

				bool foundBetterCurve = error < previousError;

				if( foundBetterCurve ) 
				{
					// store curve in bestCurve
					if(bestCurve != NULL) 
					{
						delete bestCurve;
					}
					bestError = error;
					bestCurve = curve->Clone();
				}
				else
				{
					//last iteration didn't improve situation,
					//so i guess we give up.
					break;
				}

				if( ImprovementSmallEnough( error, previousError ))
				{
 					// last iteration didn't improve curve much, so i guess
					// we're satisfied!
					break;
				}

				// ok, try to iterate through once more again - reparametrize t

				bool l_foundBetterTListStatus = false;
				ReparametrizeT ( pl, curve, tlist, l_foundBetterTListStatus );
				
				if(l_foundBetterTListStatus == false) 
				{
					// couldn't improve tlist - give up
					break;
				}
				
				delete curve;

				previousError = error;
			}
			delete curve;

			curve = bestCurve;

			bool errorTooBig = bestError > ERROR_THRESHOLD;

			if(errorTooBig) 
			{
				//divide line
				unsigned int subdivIndex = worstPoint;

				ASSERT(worstPoint < pl->Size() );

				CPolyLine * newBackLine = new CPolyLine();
				CPolyLine * newForwardLine = new CPolyLine();

#ifdef REUSE_TANGENTS
				vector<CFPoint>* backTangentList = new vector<CFPoint>;
				vector<CFPoint>* forwardTangentList = new vector<CFPoint>;
#endif
				for(unsigned int l_backPointIndex=0; 
					l_backPointIndex <= subdivIndex;
					l_backPointIndex++) 
				{
					newBackLine->Add(pl->At(l_backPointIndex)->Clone());
#ifdef REUSE_TANGENTS
					backTangentList->push_back((*tangentList)[l_backPointIndex]);
#endif
				}
				for(unsigned int l_forwardPointIndex=subdivIndex; 
					l_forwardPointIndex < pl->Size(); 
					l_forwardPointIndex++) 
				{
					newForwardLine->Add(pl->At(l_forwardPointIndex)->Clone());
#ifdef REUSE_TANGENTS
					forwardTangentList->push_back((*tangentList)[l_forwardPointIndex]);
#endif
				}
				
				l_workImage->Add(newBackLine);
				l_workImage->Add(newForwardLine);

#ifdef REUSE_TANGENTS
				hasTangents[l_workImage->Size()-2]=true;
				hasTangents[l_workImage->Size()-1]=true;
				hasTangentList.push_back(backTangentList);
				hasTangentList.push_back(forwardTangentList);
#endif
				doAddCurve=false;
			}
			delete tlist;
		}
		delete tangentList;

		if(doAddCurve) 
		{
			dst->Add(curve);
		} 
		else 
		{
			delete curve;
		}
	}

	delete l_workImage;

#ifdef MEM_DEBUG
	newMemState.Checkpoint();
	if( diffMemState.Difference( oldMemState, newMemState ) )
	{
		LOG( "Memory leaked!\n" );
	}
#endif

	return dst;
}

/** calct is supposed to distribute values from 0.0 to 1.0 
*/
vector<double>* CBezierMaker::CalcT(CPolyLine* line) const
{
	LOG ( "CalcT enter\n" );
	vector<double>* tlist = new vector<double>;

	double totalDist = 0;

	//calculate total distance
	for(unsigned int l_distancePoint = 1; 
		l_distancePoint < line->Size(); 
		l_distancePoint++) 
	{
		const double l_x1 = line->At(l_distancePoint-1)->GetX();
		const double l_y1 = line->At(l_distancePoint-1)->GetY();
		const double l_x2 = line->At(l_distancePoint)->GetX();
		const double l_y2 = line->At(l_distancePoint)->GetY();
		const double xdiff = l_x2-l_x1;
		const double ydiff = l_y2-l_y1;
		totalDist += sqrt(xdiff*xdiff+ydiff*ydiff);
	}

	double currT = 0.0;
	//first point has T = 0.0
	tlist->push_back(0.0);

	LOG( "push back 0.0\n" );

	for(unsigned int i=1; i<line->Size()-1; i++) 
	{
		const double l_x1 = line->At(i-1)->GetX();
		const double l_y1 = line->At(i-1)->GetY();
		const double l_x2 = line->At(i)->GetX();
		const double l_y2 = line->At(i)->GetY();
		const double xdiff = l_x2-l_x1;
		const double ydiff = l_y2-l_y1;
		
		currT += sqrt(xdiff*xdiff+ydiff*ydiff)/totalDist;
		tlist->push_back(currT);

		ASSERT ( currT >= 0.0 );
		ASSERT ( currT <= 1.0 );
	}

	tlist->push_back(1.0);
	LOG( "push back 1.0\n" );

	LOG( "CalcT exit\n" );

	return tlist;
}

vector<CFPoint>* CBezierMaker::CalcTangents(CPolyLine* line, const CSketchPoint* startTangent, const CSketchPoint* endTangent) const
{
	vector<CFPoint>* tlist = new vector<CFPoint>;

	for(unsigned int i=0; i<line->Size(); i++) {

		double xdiff; 
		double ydiff;

		bool usingPrecalced = false;

		if(i==0) {
			//first point
			if(startTangent == NULL) {
				xdiff = line->At(i+1)->GetX() - line->At(i)->GetX();
				ydiff = line->At(i+1)->GetY() - line->At(i)->GetY();
			} else {
				xdiff = startTangent->GetX();
				ydiff = startTangent->GetY();
				//LOG("use precalced start. x y: %f %f\n",xdiff,ydiff);
				usingPrecalced = true;
			}
		} else {
			if(i==line->Size()-1) {
				//endpoint
				if(endTangent == NULL) {
					xdiff = line->At(i)->GetX() - line->At(i-1)->GetX();
					ydiff = line->At(i)->GetY() - line->At(i-1)->GetY();
				} else {
					//use precalced tangent
					xdiff = endTangent->GetX();
					ydiff = endTangent->GetY();
					//LOG("use precalced end. x y: %f %f\n",xdiff,ydiff);
					usingPrecalced = true;
				}
			} else {
				xdiff = line->At(i+1)->GetX() - line->At(i-1)->GetX();
				ydiff = line->At(i+1)->GetY() - line->At(i-1)->GetY();
			}
		}
		double scale = sqrt(xdiff*xdiff+ydiff*ydiff);
		if(scale!=0) {
			if(!usingPrecalced) {
				xdiff/=scale;
				ydiff/=scale;
				//LOG("new tangent. x y: %f %f\n",xdiff,ydiff);
			}
		} else {
			xdiff = ydiff = 0;
		}
		tlist->push_back(CFPoint(xdiff,ydiff));
		//LOG("i %i x %f y %f\n",i,xdiff,ydiff);
	}

	return tlist;
}

CPolyLine* CBezierMaker::FitSpline(CPolyLine* pl, vector<double>* tlist, vector<CFPoint>* tangentList) const
{
	//LOG("pl->Size(): %i\n",pl->Size());
	const CFPoint startVector(pl->At(0)->GetX(),pl->At(0)->GetY());
	const CFPoint endVector(pl->At(pl->Size()-1)->GetX(),pl->At(pl->Size()-1)->GetY());

	//LOG("startVector: %f %f\n",startVector.GetX(),startVector.GetY());
	//LOG("endVector: %f %f\n",endVector.GetX(),endVector.GetY());

	CPolyLine *curve = new CPolyLine();
	curve->Add(new CSketchPoint(startVector.GetX(),startVector.GetY(),true));
	curve->Add(new CSketchPoint(endVector.GetX(),endVector.GetY(),true));

	double X_C1_det,C0_X_det,C0_C1_det;
	double alpha1,alpha2;
	double C[2][2]={{0.0,0.0},{0.0,0.0}};
	double X[2]={0.0,0.0};

	vector<CFPoint> A1v;
	vector<CFPoint> A2v;

	CFPoint t1_hat = tangentList->at(0);
	CFPoint t2_hat = -tangentList->at(tangentList->size()-1);
	//LOG("t1_hat: %f,%f\n",t1_hat.GetX(),t1_hat.GetY());
	//LOG("t2_hat: %f,%f\n",t2_hat.GetX(),t2_hat.GetY());

	for(unsigned int l_tlistIndex=0; 
		l_tlistIndex<pl->Size(); 
		l_tlistIndex++) 
	{
		CFPoint A1 = t1_hat * (3 * (*tlist)[l_tlistIndex] * (1-(*tlist)[l_tlistIndex]) * (1-(*tlist)[l_tlistIndex]));
		CFPoint A2 = t2_hat * (3 * (*tlist)[l_tlistIndex] * (*tlist)[l_tlistIndex] * (1-(*tlist)[l_tlistIndex]));
		A1v.push_back(A1);
		A2v.push_back(A2);
		//LOG("%i A1: %f %f; A2: %f %f\n",i,A1.GetX(),A1.GetY(),A2.GetX(),A2.GetY());
	}
	for(unsigned int i=0; i < pl->Size(); i++) {
		//LOG("A1v[%i]: %f %f\n",i,A1v[i].GetX(),A1v[i].GetY());
		//LOG("A2v[%i]: %f %f\n",i,A2v[i].GetX(),A2v[i].GetY());
		//LOG("(*tlist)[%i]: %f\n",i,(*tlist)[i]);

		C[0][0] += A1v[i] * A1v[i];
		C[1][0] += A1v[i] * A2v[i];
		//C[0][1] += A1v[i] * A2v[i]; do outside loop
		C[1][1] += A2v[i] * A2v[i];

		const CFPoint temp0 = startVector * (1-(*tlist)[i]) * (1-(*tlist)[i]) * (1-(*tlist)[i]);
		const CFPoint temp1 = startVector * 3.0 * (*tlist)[i] * (1-(*tlist)[i]) * (1-(*tlist)[i]);
		const CFPoint temp2 = endVector * 3.0 * (*tlist)[i] * (*tlist)[i] * (1-(*tlist)[i]);
		const CFPoint temp3 = endVector * (*tlist)[i] * (*tlist)[i] * (*tlist)[i];

		CFPoint temp = pl->At(i)->GetCoords() - (temp0+temp1+temp2+temp3);
		//LOG("%i temp: %f %f\n",i,temp.GetX(),temp.GetY());
		X[0] += temp * A1v[i];
		X[1] += temp * A2v[i];
	}
	C[0][1] = C[1][0];

	//LOG("C[0][0] %f\n",C[0][0]);
	//LOG("C[0][1] %f\n",C[0][1]);
	//LOG("C[1][1] %f\n",C[1][1]);

	X_C1_det = X[0] * C[1][1] - X[1] * C[0][1];
	C0_X_det = C[0][0] * X[1] - C[0][1] * X[0];
	C0_C1_det = C[0][0] * C[1][1] - C[0][1] * C[1][0];

	//LOG("X_C1_det: %f\n",X_C1_det);
	//LOG("C0_X_det: %f\n",C0_X_det);
	//LOG("C0_C1_det: %f\n",C0_C1_det);

	//
	//ASSERT(C0_C1_det!=0);
	if(C0_C1_det == 0) {
		//LOG("ERROR!!! do line\n");
		curve->At(0)->SetControlPointForward(startVector);
		curve->At(1)->SetControlPointBack(startVector);
	} else {
		alpha1 = X_C1_det / C0_C1_det;
		alpha2 = C0_X_det / C0_C1_det;

		//LOG("alpha1: %f\n",alpha1);
		//LOG("alpha2: %f\n",alpha2);
		curve->At(0)->SetControlPointForward(startVector + t1_hat * alpha1);
		curve->At(1)->SetControlPointBack(endVector + t2_hat * alpha2);
	}

	return curve;
}

CPolyLine* CBezierMaker::FitLine(CPolyLine* pl, const vector<CFPoint>* tangentList) const
{
	CPolyLine *curve = new CPolyLine();

	CFPoint start = pl->At(0)->GetCoords();
	CFPoint end = pl->At(1)->GetCoords();
	//CFPoint diff = end-start;
	//double dist = sqrt(diff.GetX()*diff.GetX()+diff.GetY()*diff.GetY());

	//CFPoint cp1 = start + (*tangentList)[0] * dist * 0.1;
	//CFPoint cp2 = end - (*tangentList)[1] * dist * 0.1;

	CSketchPoint *startPoint = new CSketchPoint(start.GetX(),start.GetY(),true);
	CSketchPoint *endPoint = new CSketchPoint(end.GetX(),end.GetY(),true);

	//startPoint->SetControlPointForward(cp1);
	//endPoint->SetControlPointBack(cp2);

	curve->Add(startPoint);
	curve->Add(endPoint);

	return curve;
}

/** reparametrize T list

a_foundBetterTListStatus will be set to true if we manage to 
find a better parametrization of tlist.
*/
void CBezierMaker::ReparametrizeT(CPolyLine* pl, 
								 CPolyLine* curve,
								 vector<double>* tlist,
								 bool &a_foundBetterTListStatus) const
{
	LOG( " ReparametrizeT start \n" );

	a_foundBetterTListStatus = false;

	CFPoint curveD1[3];
	CFPoint curveD2[2];
	
	/*
	LOG("curve in : %f %f; %f %f; %f %f; %f %f\n", curve->At(0)->GetX(), curve->At(0)->GetY(),
		curve->At(0)->GetControlPointForward().GetX(), curve->At(0)->GetControlPointForward().GetY(),
		curve->At(1)->GetControlPointBack().GetX(), curve->At(1)->GetControlPointBack().GetY(),
		curve->At(1)->GetX(), curve->At(1)->GetY());
	*/

	//1st derivate of curve
	curveD1[0] = curve->At(0)->GetControlPointForward() - curve->At(0)->GetCoords();
	curveD1[1] = curve->At(1)->GetControlPointBack() - curve->At(0)->GetControlPointForward();
	curveD1[2] = curve->At(1)->GetCoords() - curve->At(1)->GetControlPointBack();

	//LOG("curve' in : %f %f; %f %f; %f %f\n",curveD1[0].GetX(),curveD1[0].GetY(),curveD1[1].GetX(),curveD1[1].GetY(),curveD1[2].GetX(),curveD1[2].GetY());

	//2nd derivate of curve
	curveD2[0] = curveD1[1] - curveD1[0];
	curveD2[1] = curveD1[2] - curveD1[1];

	//LOG("curve'' in : %f %f; %f %f\n",curveD2[0].GetX(),curveD2[0].GetY(),curveD2[1].GetX(),curveD2[1].GetY());

	for(unsigned int i=0; i < pl->Size(); i++) {
		double b = (*tlist)[i];
		double a = 1 - b;

		CFPoint curvePoint0 ( curve->At(0)->GetX()*a*a*a + curve->At(0)->GetControlPointForward().GetX()*3*a*a*b + curve->At(1)->GetControlPointBack().GetX()*3*a*b*b + curve->At(1)->GetX()*b*b*b,
			curve->At(0)->GetY()*a*a*a + curve->At(0)->GetControlPointForward().GetY()*3*a*a*b + curve->At(1)->GetControlPointBack().GetY()*3*a*b*b + curve->At(1)->GetY()*b*b*b );

		CFPoint curvePoint1 ( curveD1[0].GetX()*a*a + curveD1[1].GetX()*a*b + curveD1[2].GetX()*b*b,
			curveD1[0].GetY()*a*a + curveD1[1].GetY()*a*b + curveD1[2].GetY()*b*b );
		
		CFPoint curvePoint2 ( curveD2[0].GetX()*a + curveD2[1].GetX()*b,
			curveD2[0].GetY()*a + curveD2[1].GetY()*b );

		CFPoint diff = curvePoint0 - pl->At(i)->GetCoords();

		// f(t)/f'(t)
		double numerator = diff.GetX() * curvePoint1.GetX() + diff.GetY() * curvePoint1.GetY();
		double denominator = 0;
		denominator += curvePoint1.GetX() * curvePoint1.GetX(); 
		denominator += curvePoint1.GetY() * curvePoint1.GetY();
		denominator += diff.GetX() * curvePoint2.GetX();
		denominator += diff.GetY() * curvePoint2.GetY();

		//LOG( "curvePoint1: %x %x\n", curvePoint1.GetX(), curvePoint1.GetY() );
		//LOG( "curvePoint2: %x %x\n", curvePoint2.GetX(), curvePoint2.GetY() );
		//LOG( "diff: %x %x\n", diff.GetX(), diff.GetY() );

		//calculate distances
		double oldDistance = sqrt(diff.GetX()*diff.GetX()+diff.GetY()*diff.GetY());

		if ( denominator != 0 ) 
		{
			(*tlist)[i] -= numerator/denominator;
		}
		else 
		{
			LOG( "! warn: zero denominator\n" );
		}
		b = (*tlist)[i];
		a = 1-b;
		CSketchPoint *start = curve->At(0);
		CSketchPoint *end = curve->At(1);
		CFPoint newPoint(
			start->GetX()*a*a*a + start->GetControlPointForward().GetX()*3*a*a*b + end->GetControlPointBack().GetX()*3*a*b*b + end->GetX()*b*b*b,
			start->GetY()*a*a*a + start->GetControlPointForward().GetY()*3*a*a*b + end->GetControlPointBack().GetY()*3*a*b*b + end->GetY()*b*b*b
			);
		diff = newPoint - pl->At(i)->GetCoords();
		double newDistance = sqrt(diff.GetX()*diff.GetX()+diff.GetY()*diff.GetY());

		if(newDistance > oldDistance) {
			return;
		}
	}

	//LOG("SUCCESS\n");
	a_foundBetterTListStatus = true;

	return;
}

bool CBezierMaker::ImprovementSmallEnough(double a_error, double a_previousError) const
{
	static const double REPARAM_SMALLEST_USEFUL_IMPROVEMENT = 0.1;

	double l_improvement;

	if( a_previousError != 0.0 ) 
	{
		l_improvement = 1 - a_error / a_previousError;
	} 
	else 
	{
		// previous was perfect, thus there can be no improvement 
		l_improvement = 0.0;
		ASSERT( false );
	}

	bool l_improvementSmallEnough = l_improvement < REPARAM_SMALLEST_USEFUL_IMPROVEMENT;

	return l_improvementSmallEnough;
}

void CBezierMaker::PaintImage(CSketchImage* a_image, CRawImage<ARGB> *a_canvas) const
{
	static const int SCALE = 1;
	int width = a_canvas->GetWidth();
	int height = a_canvas->GetHeight();

	CRawImage<ARGB> *tmp = new CRawImage<ARGB>(width*SCALE,height*SCALE);
	tmp->Clear();
	CLineImage *src = dynamic_cast<CLineImage*>( a_image );
	ASSERT ( src != NULL );
	CLineImagePainter::Paint(tmp,src);
	for(int i=0; i<width*height; i++) {
		ARGB p=tmp->GetPixel(i);
		if(p) a_canvas->SetPixel(i, p);
	}
	delete tmp;
}
