#pragma once

#include "RawImage.h"
#include "LineImage.h"

class CLineImageDrawer
{
public:
	CLineImageDrawer(void);
	~CLineImageDrawer(void);
	static void Draw(CRawImage* canvas, CLineImage* li);
};
