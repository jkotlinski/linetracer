/** This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

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
