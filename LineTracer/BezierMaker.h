/** This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#pragma once
#include "imageprocessor.h"

#include "LineImage.h"
#include "FPoint.h"

namespace ImageProcessing {

class CBezierMaker :
	public CImageProcessor
{
protected:
	CBezierMaker(void);
public:
	static CBezierMaker* Instance();
	~CBezierMaker(void);
	CSketchImage* Process(CProjectSettings & a_project_settings, CSketchImage* src);
private:
	CSketchImage* SimpleMethod(const CLineImage* src) const;
	double FindError(const CPolyLine* polyline, const CPolyLine* curve, vector<double> *tlist, unsigned int &worstPoint) const;
	CLineImage* DoSchneider(CProjectSettings & a_project_settings, CLineImage* src);
	vector<double>* CalcT(CPolyLine* line) const;
	vector<CFPoint>* CalcTangents(CPolyLine* line, const CSketchPoint* startTangent = NULL, const CSketchPoint* endTangent = NULL) const;
	CPolyLine* FitSpline(CPolyLine* pl, vector<double>* tlist, vector<CFPoint>* tangentList) const;
	CPolyLine* FitLine(CPolyLine* pl, const vector<CFPoint>* tangentList) const;
	void ReparametrizeT(CPolyLine* pl, CPolyLine* curve, vector<double>* tlist, bool &a_foundBetterTListStatus) const;
	bool ImprovementSmallEnough(double error, double previousError) const;
	bool TListTooUnevenlyDistributed(const vector<double> & a_tList) const;
	bool CurveTooCrazy(const CPolyLine & a_curve) const;
};

}
