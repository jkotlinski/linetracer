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