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
private:
	bool m_IsValid;
	map<const char*,double> m_Params;
public:
	void SetParam(char* name, double value);
	double GetParam(char* name);
	map<const char*,double> GetParams(void);
};
