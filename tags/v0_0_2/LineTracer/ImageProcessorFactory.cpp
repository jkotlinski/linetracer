#include "StdAfx.h"
#include ".\imageprocessorfactory.h"

CImageProcessorFactory::CImageProcessorFactory(void)
{
}

CImageProcessorFactory::~CImageProcessorFactory(void)
{
}

CBinarizer* CImageProcessorFactory::GetBinarizer(void)
{
	return NULL;
}
