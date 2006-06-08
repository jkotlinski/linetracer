#include "StdAfx.h"
#include "projectsettings.h"
#include ".\projectsettings.h"

CProjectSettings::CProjectSettings(void)
: m_params()
{
	Init();
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
	SetParam(BEZIERMAKER_ERROR_THRESHOLD, 15.0);
	SetParam(BINARIZER_THRESHOLD, -1.0);
	SetParam(BINARIZER_MEAN_C, 10.0);
	SetParam(LINESEGMENTOR_THRESHOLD, 1.5);
	SetParam(TAILPRUNER_THRESHOLD, 4.0);
	SetParam(SKELETONIZER_SCALE, 1.0);
	SetParam(KNEESPLITTER_THRESHOLD, -0.3);
	SetParam(HOLEFILLER_MAX_AREA_TO_FILL, 10.0);
	SetParam(GAUSSIAN_RADIUS, 0.5);
}

void CProjectSettings::Reset(void)
{
	Init();
}
