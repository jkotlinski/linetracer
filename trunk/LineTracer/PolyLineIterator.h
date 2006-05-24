#pragma once

#include "PolyLine.h"
#include "FPoint.h"

class PolyLineIterator
{
public:
	PolyLineIterator(CPolyLine * a_line, CFPoint a_start_point);
	~PolyLineIterator(void);

	/* returns true if everything was ok, false if we already reached
	the last point in line and thus couldn't advance further */
	CSketchPoint * PolyLineIterator::Next( bool & l_status );
	CSketchPoint * PolyLineIterator::Next( );
private:
	CPolyLine * m_line;
	int m_index;
	bool m_is_forward_iterator;
};
