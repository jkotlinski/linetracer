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
	CSketchImage* SimpleMethod(const CLineImage* src) const;
	double FindError(const CPolyLine* polyline, const CPolyLine* curve, vector<double> *tlist, unsigned int &worstPoint) const;
	CLineImage* DoSchneider(const CLineImage* src) const;
	vector<double>* CalcT(CPolyLine* line) const;
	vector<CFPoint>* CalcTangents(CPolyLine* line, const CSketchPoint* startTangent = NULL, const CSketchPoint* endTangent = NULL) const;
	CPolyLine* FitSpline(CPolyLine* pl, vector<double>* tlist, vector<CFPoint>* tangentList) const;
	CPolyLine* FitLine(CPolyLine* pl, const vector<CFPoint>* tangentList) const;
	void ReparametrizeT(CPolyLine* pl, CPolyLine* curve, vector<double>* tlist, bool &a_foundBetterTListStatus) const;
	bool ImprovementSmallEnough(double error, double previousError) const;
	bool TListTooUnevenlyDistributed(const vector<double> & a_tList) const;
	bool CurveTooCrazy(const CPolyLine & a_curve) const;
};
