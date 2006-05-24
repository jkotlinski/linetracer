#include "StdAfx.h"
#include ".\polylineiterator.h"
#include <assert.h>

PolyLineIterator::PolyLineIterator(CPolyLine * a_line, CFPoint a_start_point)
: m_line(a_line)
, m_index(0)
, m_is_forward_iterator(false)
{
	assert ( a_line );
	if ( a_line->GetHeadPoint()->GetCoords() == a_start_point )
	{
		m_is_forward_iterator = true;
	} else {
		m_index = a_line->Size() - 1;
	}
}

PolyLineIterator::~PolyLineIterator(void)
{
	m_line = 0;
}

CSketchPoint * PolyLineIterator::Next ( bool & l_status ) {
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
