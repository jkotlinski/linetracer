#include "StdAfx.h"
#include ".\imageprocessor.h"

CImageProcessor::CImageProcessor(void)
: m_IsActive(false)
, m_IsValid(false)
{
}

CImageProcessor::~CImageProcessor(void)
{
}

void CImageProcessor::SetActive(bool state)
{
	m_IsActive=state;
}

bool CImageProcessor::IsActive(void)
{
	return m_IsActive;
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
