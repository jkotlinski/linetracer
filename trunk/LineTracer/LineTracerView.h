// LineTracerView.h : interface of the CLineTracerView class
//


#pragma once

#include "LineTracerDoc.h"
#include "ProjectSettings.h"

#define WM_UPDATE_TOOLBOX_DATA_FROM_LAYERS (WM_USER+0x100)

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
protected:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CLineTracerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Gnerated message map functions
public:
	void OnInitialUpdate(void);
protected:
 	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnFileOpenimage();
	afx_msg void OnUpdateViewSkeletonizer(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewBinarizer(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewGaussian(CCmdUI *pCmdUI);
	afx_msg void OnFileExporteps();
	afx_msg void OnUpdateViewOriginal(CCmdUI *pCmdUI);
	afx_msg void OnUpdateZoom200(CCmdUI *pCmdUI);
	afx_msg void OnUpdateZoom100(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewBeziermaker(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewThinner(CCmdUI *pCmdUI);
	
	DECLARE_MESSAGE_MAP()
private:
	void ProcessLayers(void) const;
public:
	void HandleChangedToolboxParam(CLayerManager::LayerTypes a_layerId, 
		CProjectSettings::ParamName a_paramName);
};

#ifndef _DEBUG  // debug version in LineTracerView.cpp
inline CLineTracerDoc* CLineTracerView::GetDocument() const
   { return reinterpret_cast<CLineTracerDoc*>(m_pDocument); }
#endif

