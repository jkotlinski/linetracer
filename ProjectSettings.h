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
