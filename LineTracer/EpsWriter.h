#pragma once

#include "LineImage.h"

class CEpsWriter
{
public:
	CEpsWriter(void);
	~CEpsWriter(void);
	static void Write(CLineImage* lineImage);
};
