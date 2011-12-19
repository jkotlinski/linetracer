/** This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

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
	bool m_is_valid;
	bool m_is_forward_iterator;
};
