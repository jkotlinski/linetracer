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
#include "imageprocessor.h"

CImageProcessor::CImageProcessor()
: m_IsValid(false)
, m_name("???")
, m_type ( INVALID_TYPE )
{
	//LOG("init imageprocessor\n");
}

CImageProcessor::~CImageProcessor(void)
{
}

const CString *CImageProcessor::GetName(void)
{
	return &m_name;
}

void CImageProcessor::SetName(const CString &a_name)
{
	m_name = a_name;
}

void CImageProcessor::SetType(enum Type a_type) {
	m_type = a_type;
}

CImageProcessor::Type CImageProcessor::GetType ( void ) {
	ASSERT ( m_type != INVALID_TYPE );
	return m_type;
}
