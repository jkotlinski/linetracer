#pragma once

#include "LineImage.h"

class CEpsWriter
{
public:
	~CEpsWriter(void);
	static void Write(CString *FileName);
	static CEpsWriter* Instance();
protected:
	CEpsWriter(void);
};
