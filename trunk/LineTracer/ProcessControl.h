#pragma once

class CProcessControl
{
private:
	CProcessControl(void);
	~CProcessControl(void);
public:
	CProcessControl* Instance(void);
	volatile int m_isProcessing;
};
