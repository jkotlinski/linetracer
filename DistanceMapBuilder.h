#pragma once
#include "imageprocessor.h"

class DistanceMapBuilder
{
private:
	DistanceMapBuilder(void);
public:
	static DistanceMapBuilder* Instance();
	~DistanceMapBuilder(void);

	const CRawImage<int>* GetDistanceMap() const;
	void CalcDistanceMap(const CRawImage<bool>* img);

private:
	CRawImage<int> * m_distanceMap;
};
