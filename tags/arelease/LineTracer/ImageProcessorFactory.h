#pragma once
#include "binarizer.h"

class CImageProcessorFactory
{
public:
	CImageProcessorFactory(void);
	~CImageProcessorFactory(void);
	CBinarizer* GetBinarizer(void);
private:
	CBinarizer m_Binarizer;
};
