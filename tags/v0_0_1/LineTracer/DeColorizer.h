#pragma once
#include "imageprocessor.h"

class CDeColorizer :
	public CImageProcessor
{
public:
	~CDeColorizer(void);
	static CDeColorizer* Instance(void);
protected:
	CDeColorizer(void);
	static CDeColorizer* _instance;
public:
	CRawImage* Process(CRawImage* src);
};
