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
	double FindError(CPolyLine* polyline, CPolyLine* curve, vector<double> *tlist, unsigned int &worstPoint) const;
	CLineImage* DoSchneider(const CLineImage* src);
	vector<double>* CalcT(CPolyLine* line) const;
	vector<CFPoint>* CalcTangents(CPolyLine* line, const CSketchPoint* startTangent = NULL, const CSketchPoint* endTangent = NULL) const;
	CPolyLine* FitSpline(CPolyLine* pl, vector<double>* tlist, vector<CFPoint>* tangentList) const;
	CPolyLine* FitLine(CPolyLine* pl, const vector<CFPoint>* tangentList) const;
	void ReparametrizeT(CPolyLine* pl, CPolyLine* curve, vector<double>* tlist, bool &a_foundBetterTListStatus) const;
	bool ImprovementSmallEnough(double error, double previousError) const;
public:
	void PaintImage(CSketchImage* a_image, CRawImage<ARGB> *a_canvas) const;
};