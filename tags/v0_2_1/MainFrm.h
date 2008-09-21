// MainFrm.h : interface of the CMainFrame class
//


#pragma once
#include "afxext.h"
#include "ToolBox.h"

class CMainFrame : public CFrameWnd
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	string m_statustext;

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	//CToolBox	m_wndToolbox;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose(void);
	afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
	void ActivateFrame(int nCmdShow);

private:
	void WriteWindowPlacement(WINDOWPLACEMENT *pwp);
	BOOL ReadWindowPlacement(WINDOWPLACEMENT *pwp);

};
