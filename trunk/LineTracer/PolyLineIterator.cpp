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

#include "StdAfx.h"
#include ".\polylineiterator.h"
#include <assert.h>

PolyLineIterator::PolyLineIterator(const CPolyLine * const a_line, CFPoint a_start_point, bool & a_status)
: m_line(a_line)
, m_index(0)
, m_is_forward_iterator(false)
{
	a_status=true;
	assert ( a_line );
	CSketchPoint * l_headpoint = a_line->GetHeadPoint();
	CSketchPoint * l_tailpoint = a_line->GetTailPoint();
	if ( l_headpoint->GetCoords( ) == a_start_point )
	{
		m_is_forward_iterator = true;
	} else if ( l_tailpoint->GetCoords() == a_start_point ) {
		m_index = a_line->Size() - 1;
	} else {
		a_status = false;
	}
	m_is_valid = a_status;
}

PolyLineIterator::~PolyLineIterator(void)
{
}

CSketchPoint * PolyLineIterator::Next ( bool & l_status ) {
	assert ( m_is_valid );
	l_status = true;
	if ( m_index < 0 ) {
		m_index = 0;
		l_status = false;
	} 
	else if ( static_cast<unsigned int>(m_index) >= m_line->Size() ) {
		m_index = m_line->Size() - 1;
		l_status = false;
	}
	CSketchPoint * a_p = m_line->At(m_index);
	m_index += m_is_forward_iterator ? 1 : -1;
	return a_p;
}

CSketchPoint * PolyLineIterator::Next ( ) {
	bool l_status = false;
	return Next ( l_status );
}

PolyLineIterator * PolyLineIterator::CreateIteratorFromOtherEnd()
{
	bool l_status;
	assert ( m_is_valid );
	int l_index = m_is_forward_iterator ? m_line->Size() - 1 : 0;
	return new PolyLineIterator(m_line, m_line->At(l_index)->GetCoords(), l_status);
}

int PolyLineIterator::PointsLeftToIterate () {
	assert ( m_is_valid );
	if ( m_is_forward_iterator ) {
		return m_line->Size() - 1 - m_index;
	} else {
		return m_index;
	}
}
