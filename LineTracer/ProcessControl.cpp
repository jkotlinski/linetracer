#include "StdAfx.h"
#include ".\processcontrol.h"

CProcessControl::CProcessControl(void)
: m_isProcessing(0)
{
}

CProcessControl::~CProcessControl(void)
{
}

CProcessControl* CProcessControl::Instance(void)
{
	static CProcessControl _inst;
	return &_inst;
}
