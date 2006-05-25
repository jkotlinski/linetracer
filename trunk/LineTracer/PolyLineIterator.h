#pragma once

#include "PolyLine.h"
#include "FPoint.h"

class PolyLineIterator
{
public:
	// a_status is set to false if line doesn't start or end at a_start_point
	PolyLineIterator(const CPolyLine * const a_line, CFPoint a_start_point, bool & a_status );
	~PolyLineIterator(void);

	/* returns true if everything was ok, false if we already reached
	the last point in line and thus couldn't advance further */
	CSketchPoint * PolyLineIterator::Next( bool & l_status );
	CSketchPoint * PolyLineIterator::Next( );

	PolyLineIterator * PolyLineIterator::CreateIteratorFromOtherEnd();

	int PolyLineIterator::PointsLeftToIterate();
private:
	const CPolyLine * const m_line;
	int m_index;
	bool m_is_forward_iterator;
	bool m_is_valid;
};
