#pragma once

#include <hash_map>
#include "RawImage.h"

using namespace stdext;

class CImageProcessor
{
public:
	CImageProcessor(void);
	~CImageProcessor(void);
private:
	bool m_IsActive;
public:
	void SetActive(bool state);
	bool IsActive(void);
	virtual CSketchImage* Process(CSketchImage* src) = 0;
private:
	bool m_IsValid;
	hash_map<const char*,double> m_Params;
public:
	void SetParam(char* name, double value);
	double GetParam(char* name);
	hash_map<const char*,double> GetParams(void);
};
