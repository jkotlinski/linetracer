#pragma once
#include "imageprocessor.h"

#include "LineImage.h"
#include "FPoint.h"

class CBezierMaker :
	public CImageProcessor
{
protected:
	CBezierMaker(void);
public:
	static CBezierMaker* Instance();
	~CBezierMaker(void);
	CSketchImage* Process(CSketchImage* src);
private:
	CSketchImage* SimpleMethod(CLineImage* src);
	double FindError(CPolyLine* polyline, CPolyLine* curve, vector<double> *tlist, int &worstPoint);
	CLineImage* DoSchneider(CLineImage* src);
	vector<double>* CalcT(CPolyLine* line);
	vector<CFPoint>* CalcTangents(CPolyLine* line,CSketchPoint* startTangent = NULL, CSketchPoint* endTangent = NULL);
	CPolyLine* FitSpline(CPolyLine* pl, vector<double>* tlist, vector<CFPoint>* tangentList);
	CPolyLine* FitLine(CPolyLine* pl, vector<CFPoint>* tangentList);
	bool Reparametrize(CPolyLine* pl, CPolyLine* curve, vector<double>* tlist);
};
