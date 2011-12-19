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

//////////////////////////////////////////////////////////////////////
//
// InitDialogBar.h: interface for the CInitDialogBar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INITDIALOGBAR_H__46B4D2B3_C982_11D1_8902_0060979C2EFD__INCLUDED_)

#define AFX_INITDIALOGBAR_H__46B4D2B3_C982_11D1_8902_0060979C2EFD__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

////////////////////////////////////////////////////////////////////////////
//
// CInitDialogBar window
//
////////////////////////////////////////////////////////////////////////////

class CInitDialogBar : public CDialogBar
{
	DECLARE_DYNAMIC(CInitDialogBar)

	// Construction / Destruction
	public:
	CInitDialogBar();
	virtual ~CInitDialogBar();

	// Attributes
	public:

	// Operations
	public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInitDialogBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	// Implementation
	public:
	BOOL Create(CWnd * pParentWnd, UINT nIDTemplate, UINT nStyle, UINT
	nID);
	BOOL Create(CWnd * pParentWnd, LPCTSTR lpszTemplateName, UINT
	nStyle, UINT nID);

	protected:
	virtual BOOL OnInitDialogBar();

	// Generated message map functions
	protected:
	//{{AFX_MSG(CInitDialogBar)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_INITDIALOGBAR_H__46B4D2B3_C982_11D1_8902_0060979C2EFD__INCLUDED_)
