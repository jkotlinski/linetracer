#include "StdAfx.h"
#include ".\imageprocessor.h"

CImageProcessor::CImageProcessor(void)
: m_IsValid(false)
{
}

CImageProcessor::~CImageProcessor(void)
{
}

void CImageProcessor::SetParam(char* name, double value)
{
	m_Params[name]=value;
}

double CImageProcessor::GetParam(char* name)
{
	return m_Params[name];
}

hash_map<const char*,double> CImageProcessor::GetParams(void)
{
	return m_Params;
}
