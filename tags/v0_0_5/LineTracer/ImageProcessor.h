#pragma once

#include <map>
#include "RawImage.h"

using namespace std;

class CImageProcessor
{
public:
	CImageProcessor(void);
	virtual ~CImageProcessor(void);

	virtual CSketchImage* Process(CSketchImage* src) = 0;
	virtual void PaintImage(CSketchImage *a_image, CRawImage<ARGB> *a_canvas) const = 0;

public:
	enum ParamName {
		BEZIERMAKER_ERROR_THRESHOLD,
		BINARIZER_THRESHOLD,
		BINARIZER_MEAN_C,
		GAUSSIAN_RADIUS,
		HOLEFILLER_MIN_AREA,
		KNEESPLITTER_THRESHOLD,
		LINESEGMENTOR_THRESHOLD,
		SKELETONIZER_SCALE,
		TAILPRUNER_THRESHOLD
	};

private:
	CString *m_name;
	bool m_IsValid;
	map<enum ParamName,double> m_Params;
public:
	void SetParam(ParamName name, double value);
	double GetParam(ParamName name);
	map<enum ParamName,double> GetParams(void);
	CString* GetName(void);
	void SetName(CString* a_name);
};
