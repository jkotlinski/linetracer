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

#pragma once

#include <map>

using namespace std;

class CProjectSettings
{
public:
	CProjectSettings(void);
	~CProjectSettings(void);

	enum ParamName {
		PARAMNAME_INVALID_START,
		BEZIERMAKER_ERROR_THRESHOLD,
		BINARIZER_THRESHOLD,
		BINARIZER_MEAN_C,
		GAUSSIAN_RADIUS,
		HOLEFILLER_MAX_AREA_TO_FILL,
		KNEESPLITTER_THRESHOLD,
		//LINESEGMENTOR_THRESHOLD,
		SKELETONIZER_SCALE,
		TAILPRUNER_THRESHOLD,
		PARAMNAME_INVALID_END
	};

	double GetParam ( CProjectSettings::ParamName a_paramName );
	void SetParam ( CProjectSettings::ParamName a_paramName, double a_value );

private:
	map<int,double> m_params;
public:
	void Init(void);
	void Reset(void);
	CProjectSettings * Clone(void);
};
