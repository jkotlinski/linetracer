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

double CProjectSettings::GetParam(CProjectSettings::ParamName name) {
	//LOG ( "settings->GetParam; name = %i\n", name );
	//LOG ( "m_params.count(name): %i\n", m_params.count(name) );
	if ( m_params.count ( name ) == 0 ) {
		return -1.0;
	}
	double l_retval = m_params[name];
	//TRACE ( "settings->GetParam; retval = %f\n", l_retval );
	return l_retval;
}

void CProjectSettings::SetParam ( CProjectSettings::ParamName a_paramName, double a_value ) {
	//TRACE ( "settings->SetParam %i = %f\n", a_paramName, a_value );
	m_params[a_paramName] = a_value;
}

void CProjectSettings::Init(void)
{
	SetParam(BEZIERMAKER_ERROR_THRESHOLD, 15.0);
	SetParam(BINARIZER_THRESHOLD, -1.0);
	SetParam(BINARIZER_MEAN_C, 10.0);
	//SetParam(LINESEGMENTOR_THRESHOLD, 1.5);
	SetParam(TAILPRUNER_THRESHOLD, 4.0);
	SetParam(SKELETONIZER_SCALE, 1.0);
	SetParam(KNEESPLITTER_THRESHOLD, -0.3);
	SetParam(HOLEFILLER_MAX_AREA_TO_FILL, 10.0);
}

void CProjectSettings::Reset(void)
{
	Init();
}

CProjectSettings * CProjectSettings::Clone(void)
{
	CProjectSettings * l_settings = DEBUG_NEW CProjectSettings();

	for ( int i = (int)(PARAMNAME_INVALID_START + 1);
		i < (int)PARAMNAME_INVALID_END; 
		i++ ) {
		l_settings->SetParam( 
			static_cast<ParamName>(i), 
			GetParam( static_cast<ParamName>(i) ) );
	}

	return l_settings;
}
