#include "StdAfx.h"
#include "projectsettings.h"

CProjectSettings::CProjectSettings(void)
: m_params()
{
}

CProjectSettings::~CProjectSettings(void)
{
}

CProjectSettings* CProjectSettings::Instance(void) {
	static CProjectSettings l_instance;
	return &l_instance;
}

double CProjectSettings::GetParam(CProjectSettings::ParamName name) {
	LOG ( "settings->GetParam; name = %i\n", name );
	LOG ( "m_params.count(name): %i\n", m_params.count(name) );
	ASSERT ( m_params.count(name) == 1 );

	if ( m_params.count ( name ) == 0 ) {
		return -1.0;
	}
	double l_retval = m_params[name];
	LOG ( "settings->GetParam; retval = %f\n", l_retval );
	return l_retval;
}

void CProjectSettings::SetParam ( CProjectSettings::ParamName a_paramName, double a_value ) {
	LOG ( "settings->SetParam %i = %f\n", a_paramName, a_value );
	m_params[a_paramName] = a_value;
}

void CProjectSettings::Init(void)
{
	//m_params.clear();
	SetParam(BINARIZER_THRESHOLD,-1.0);
}
