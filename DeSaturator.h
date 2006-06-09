#pragma once
#include "imageprocessor.h"

class CDeSaturator :
	public CImageProcessor
{
public:
	~CDeSaturator(void);
	static CDeSaturator* Instance(void);
protected:
	CDeSaturator(void);
public:
	CSketchImage* Process(CProjectSettings & a_project_settings, CSketchImage* src);
};
