#pragma once
#include "imageprocessor.h"

class CTailPruner :
	public CImageProcessor
{
protected:
	CTailPruner(void);
public:
	static CTailPruner* Instance();
	~CTailPruner(void);
	CSketchImage* Process(CSketchImage* src);
};
