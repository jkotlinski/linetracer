////////////////////////////////////////////////////////////////////////////
//
// CInitDialogBar window
//
////////////////////////////////////////////////////////////////////////////

#include "InitDialogBar.h"

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
	BOOL Create(CWnd * pParentWnd, UINT nIDTemplate, UINT nStyle, UINT nID);
	BOOL Create(CWnd * pParentWnd, LPCTSTR lpszTemplateName, UINT
	nStyle, UINT nID);

	protected:
	virtual BOOL OnInitDialogBar();

	// Generated message map functions
	protected:
	//{{AFX_MSG(CInitDialogBar)
	// NOTE - the ClassWizard will add and remove member functions
	here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////
/

#endif // !defined(AFX_INITDIALOGBAR_H__46B4D2B3_C982_11D1_8902_0060979C2EFD__INCLUDED_)

////////////////////////////////////////////////////////////////////////////
//
// InitDialogBar.cpp: implementation of the CInitDialogBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "InitDialogBar.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CInitDialogBar, CDialogBar)

BEGIN_MESSAGE_MAP(CInitDialogBar, CDialogBar)
//{{AFX_MSG_MAP(CInitDialogBar)
// NOTE - the ClassWizard will add and remove mapping macros here.
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CInitDialogBar::CInitDialogBar()
{
	// In derived classes set intial
	// state of control(s) here
}

CInitDialogBar::~CInitDialogBar()
{

}

BOOL CInitDialogBar::Create(CWnd * pParentWnd, LPCTSTR lpszTemplateName,
	UINT nStyle, UINT nID)
{
	// Let MFC Create the control
	if(!CDialogBar::Create(pParentWnd, lpszTemplateName, nStyle, nID))
		return FALSE;

	// Since there is no WM_INITDIALOG message we have to call
	// our own InitDialog function ourselves after m_hWnd is valid
	if(!OnInitDialogBar())
		return FALSE;

	return TRUE;
}

BOOL CInitDialogBar::Create(CWnd * pParentWnd, UINT nIDTemplate,
	UINT nStyle, UINT nID)
{
	if(!Create(pParentWnd, MAKEINTRESOURCE(nIDTemplate), nStyle, nID))
		return FALSE;

	// Since there is no WM_INITDIALOG message we have to call
	// our own InitDialog function ourselves after m_hWnd is valid
	if(!OnInitDialogBar())
		return FALSE;
	return TRUE;
}

BOOL CInitDialogBar::OnInitDialogBar()
{
	// Support for the MFC DDX model 
	// If you do not want this do not call the base class
	// from derived classes
	UpdateData(FALSE);

	return TRUE;
}

void CInitDialogBar::DoDataExchange(CDataExchange* pDX)
{
	//Derived Classes Overide this function
	ASSERT(pDX);

	CDialogBar::DoDataExchange(pDX);

	// In derived class call the DDX_??? functions to set/retrieve values
	// of your controls. See example derived class for how to do this.
}
