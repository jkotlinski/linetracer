// LineTracerView.h : interface of the CLineTracerView class
//


#pragma once


class CLineTracerView : public CScrollView
{
protected: // create from serialization only
	CLineTracerView();
	DECLARE_DYNCREATE(CLineTracerView)

// Attributes
public:
	CLineTracerDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CLineTracerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileOpenimage();
	void OnInitialUpdate(void);
	afx_msg void OnUpdateViewSkeletonizer(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewBinarizer(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewGaussian(CCmdUI *pCmdUI);
	afx_msg void OnFileExporteps();
};

#ifndef _DEBUG  // debug version in LineTracerView.cpp
inline CLineTracerDoc* CLineTracerView::GetDocument() const
   { return reinterpret_cast<CLineTracerDoc*>(m_pDocument); }
#endif

