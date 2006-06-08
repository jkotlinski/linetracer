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

	// Tell the frame window to forbid docking.
    EnableDocking ( 0 );

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// Create the dialog bar using one of its Create() methods
	if( !CToolBox::Instance()->Create(this,
			          IDD_TOOLBOX,
			          CBRS_LEFT | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY,
			          IDD_TOOLBOX) )
	{
		TRACE0(_T("Failed to create the toolbox\n"));
		return -1;
	}

	// When the dialog bar is undocked, display a caption on its title bar
	CToolBox::Instance()->SetWindowText("Toolbox");

	// disallow docking -- was: Allow the dialog bar to be dockable on the left or the right of the frame
	CToolBox::Instance()->EnableDocking(0);
	//DockControlBar(&m_wndToolbox);
	
	//LoadBarState(_T("ToolbarState"));
	FloatControlBar(CToolBox::Instance(), CPoint(0,0), CBRS_ALIGN_TOP);

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

    WINDOWPLACEMENT    wp;

    // before it is destroyed, save the position of the window
    wp.length = sizeof wp;

    if ( GetWindowPlacement(&wp) )
    {
        if ( IsIconic() )
          // never restore to Iconic state
          wp.showCmd = SW_SHOW ;

        if ((wp.flags & WPF_RESTORETOMAXIMIZED) != 0)
          // if maximized and maybe iconic restore maximized state
          wp.showCmd = SW_SHOWMAXIMIZED ;

        // and write it to the .INI file
        WriteWindowPlacement(&wp);
    }
    
    CFrameWnd::OnClose();
}

void CMainFrame::ActivateFrame(int nCmdShow)
{
    // nCmdShow is the normal show mode this frame should be in
    // translate default nCmdShow (-1)
    if (nCmdShow == -1)
    {
        if (!IsWindowVisible())
            nCmdShow = SW_SHOWNORMAL;
        else if (IsIconic())
            nCmdShow = SW_RESTORE;
    }

    // bring to top before showing
    BringToTop(nCmdShow);

    if (nCmdShow != -1)
    {
        // show the window as specified
      WINDOWPLACEMENT wp;
      
      if ( !ReadWindowPlacement(&wp) )
      {
          ShowWindow(nCmdShow);
      }
      else
      {
         if ( nCmdShow != SW_SHOWNORMAL )  
           wp.showCmd = nCmdShow;

         SetWindowPlacement(&wp);
         // ShowWindow(wp.showCmd);
      }

      // and finally, bring to top after showing
      BringToTop(nCmdShow);
    }

	CRect l_screenRect;
	GetWindowRect(&l_screenRect);
	FloatControlBar(CToolBox::Instance(), CPoint(l_screenRect.right,l_screenRect.top), CBRS_ALIGN_TOP);

	return ;
}

static char szSection[]   = "Settings";
static char szWindowPos[] = "WindowPos";
static char szFormat[] = "%u,%u,%d,%d,%d,%d,%d,%d,%d,%d";

BOOL CMainFrame::ReadWindowPlacement(WINDOWPLACEMENT *pwp)
{
    CString strBuffer;
    int    nRead ;

    strBuffer = AfxGetApp()->GetProfileString(szSection, szWindowPos);
    if ( strBuffer.IsEmpty() )  return FALSE;

    nRead = sscanf(strBuffer, szFormat,
                &pwp->flags, &pwp->showCmd,
                &pwp->ptMinPosition.x, &pwp->ptMinPosition.y,
                &pwp->ptMaxPosition.x,&pwp->ptMaxPosition.y,
                &pwp->rcNormalPosition.left,  &pwp->rcNormalPosition.top,
                &pwp->rcNormalPosition.right, &pwp->rcNormalPosition.bottom);
    if ( nRead != 10 )  return FALSE;
    pwp->length = sizeof(WINDOWPLACEMENT);

    return TRUE;
}

// Write a window placement to settings section of app's ini file.

void CMainFrame::WriteWindowPlacement(WINDOWPLACEMENT *pwp)
{
    char szBuffer[sizeof("-32767")*8 + sizeof("65535")*2];
    int max = 1;
    CString s;

    sprintf(szBuffer, szFormat,
            pwp->flags, pwp->showCmd,
            pwp->ptMinPosition.x, pwp->ptMinPosition.y,
            pwp->ptMaxPosition.x, pwp->ptMaxPosition.y,
            pwp->rcNormalPosition.left, pwp->rcNormalPosition.top,
            pwp->rcNormalPosition.right, pwp->rcNormalPosition.bottom);
     AfxGetApp()->WriteProfileString(szSection, szWindowPos, szBuffer);

}
