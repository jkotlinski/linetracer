#include "StdAfx.h"
#include "imageprocessor.h"

CImageProcessor::CImageProcessor()
: m_IsValid(false)
, m_name(NULL)
{
	LOG("init imageprocessor\n");
}

CImageProcessor::~CImageProcessor(void)
{
	try {
		LOG("~CImageProcessor()\n");
	} catch (...) {
	}
	if ( m_name != NULL )
	{
		delete m_name;
	}
}

void CImageProcessor::SetParam(ParamName name, double value)
{
	m_Params[name]=value;
}

double CImageProcessor::GetParam(ParamName name)
{
	ASSERT ( m_Params.count(name) == 1 );
	return m_Params[name];
}

map<CImageProcessor::ParamName,double> CImageProcessor::GetParams(void)
{
	return m_Params;
}

CString* CImageProcessor::GetName(void)
{
	return NULL;
}

void CImageProcessor::SetName(CString* a_name)
{
	if ( m_name != NULL )
	{
		delete m_name;
	}
	m_name = a_name;
}

