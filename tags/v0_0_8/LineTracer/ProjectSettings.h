#pragma once

#include <map>

using namespace std;

class CProjectSettings
{
private:
	//lint -e{1704} singleton
	CProjectSettings(void);
public:
	static CProjectSettings* Instance();
	~CProjectSettings(void);

	enum ParamName {
		PARAMNAME_INVALID_START,
		BEZIERMAKER_ERROR_THRESHOLD,
		BINARIZER_THRESHOLD,
		BINARIZER_MEAN_C,
		GAUSSIAN_RADIUS,
		HOLEFILLER_MIN_AREA,
		KNEESPLITTER_THRESHOLD,
		LINESEGMENTOR_THRESHOLD,
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
};
