// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "LineTracer.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Tell the frame window to permit docking.
    EnableDocking (CBRS_ALIGN_ANY);

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CToolBox *m_wndToolbox = CToolBox::Instance();

	// Create the dialog bar using one of its Create() methods
	if( !m_wndToolbox->Create(this,
			          IDD_TOOLBOX,
			          CBRS_LEFT | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY,
			          IDD_TOOLBOX) )
	{
		TRACE0(_T("Failed to create the toolbox\n"));
		return -1;
	}

	// When the dialog bar is undocked, display a caption on its title bar
	m_wndToolbox->SetWindowText("Toolbox");


	// Allow the dialog bar to be dockable on the left or the right of the frame
	m_wndToolbox->EnableDocking(0);
	//DockControlBar(&m_wndToolbox);
	FloatControlBar(m_wndToolbox, CPoint(0,0), CBRS_ALIGN_TOP);

	//LoadBarState(_T("ToolbarState"));

	m_wndToolbox->Init();

    return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

afx_msg void CMainFrame::OnClose(void)
{
	SaveBarState(_T("ToolbarState"));
	CFrameWnd::OnClose();
}
