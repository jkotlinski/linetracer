#include "ImageProcessor.h"

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

const wxString *CImageProcessor::GetName(void)
{
	return &m_name;
}

void CImageProcessor::SetName(const wxString &a_name)
{
	m_name = a_name;
}

void CImageProcessor::SetType(enum Type a_type) {
	m_type = a_type;
}

CImageProcessor::Type CImageProcessor::GetType ( void ) {
	wxASSERT ( m_type != INVALID_TYPE );
	return m_type;
}
