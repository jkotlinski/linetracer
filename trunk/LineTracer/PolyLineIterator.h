#pragma once

#include "PolyLine.h"
#include "FPoint.h"

class PolyLineIterator
{
public:
	PolyLineIterator(CPolyLine * a_line, CFPoint a_start_point);
	~PolyLineIterator(void);

	CSketchPoint * Next();

private:
	CPolyLine * m_line;
	int m_index;
	bool m_is_forward_iterator;
};
