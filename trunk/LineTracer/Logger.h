#pragma once

class CLogger
{
public:
	static CLogger* Instance(void);
	static void Inactivate(void);
	static void Activate(void);
	void Log(LPCSTR pszFormat, ...);
private:
	//lint -e{1704} singleton
	CLogger(void);
	~CLogger(void);
	FILE* m_outFile;
	bool m_active;
};
