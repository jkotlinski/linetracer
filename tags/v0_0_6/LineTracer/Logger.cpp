#include "StdAfx.h"
#include "Logger.h"

#include <atltrace.h>

CLogger::CLogger(void)
: m_outFile(NULL)
, m_active(true)
{
	m_outFile = fopen( "log.txt", "w" );
}

CLogger::~CLogger(void)
{
	if ( m_outFile != NULL) {
		fclose ( m_outFile );
	}
}

CLogger* CLogger::Instance()
{
	static CLogger k_instance;
	return &k_instance;
}

void CLogger::Inactivate(void) {
	Instance()->m_active = false;
}

void CLogger::Activate(void) {
	Instance()->m_active = true;
}

void CLogger::Log(LPCSTR pszFormat, ...) {
	if ( m_active == false ) {
		return;
	}
	va_list ptr;
	va_start(ptr, pszFormat);
	(void) TRACE (pszFormat, ptr);

	ASSERT ( m_outFile != NULL );
	(void) vfprintf( m_outFile, pszFormat, ptr );

	va_end(ptr);
}
