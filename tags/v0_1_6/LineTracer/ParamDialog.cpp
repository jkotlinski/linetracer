// ParamDialog.cpp : implementation file
//

#include "stdafx.h"
#include "LineTracer.h"
#include "ParamDialog.h"


// CParamDialog dialog

IMPLEMENT_DYNAMIC(CParamDialog, CDialog)
CParamDialog::CParamDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CParamDialog::IDD, pParent)
	, m_EditValue(_T(""))
{
}

CParamDialog::~CParamDialog()
{
}

void CParamDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT, m_EditValue);
}


BEGIN_MESSAGE_MAP(CParamDialog, CDialog)
END_MESSAGE_MAP()


// CParamDialog message handlers

BOOL CParamDialog::OnInitDialog(void)
{
	CDialog::OnInitDialog();
	CEdit *editBox;
	editBox = static_cast<CEdit*> (GetDlgItem(IDC_EDIT));
	(void) editBox->SetFocus();
	editBox->SetSel(0,m_EditValue.GetLength());
	return false;
}