#pragma once


// CParamDialog dialog

class CParamDialog : public CDialog
{
	DECLARE_DYNAMIC(CParamDialog)

public:
	CParamDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CParamDialog();

// Dialog Data
	enum { IDD = IDD_PARAMDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_EditValue;
	BOOL OnInitDialog(void);
};
