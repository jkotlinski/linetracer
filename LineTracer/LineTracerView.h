// LineTracerView.h : interface of the CLineTracerView class
//


#pragma once

#include "LineTracerDoc.h"
#include "ProjectSettings.h"

#include "Magnification.h"
#include "AffineTransform.h"
#include "atltypes.h"

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
	afx_msg void OnUpdateViewBeziermaker(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewThinner(CCmdUI *pCmdUI);
	
	DECLARE_MESSAGE_MAP()
private:
	void ProcessLayers(void);
public:
	void HandleChangedToolboxParam(CLayerManager::LayerTypes a_layerId, 
		CProjectSettings::ParamName a_paramName);
private:
	float GetXTranslation(void);	
	float GetYTranslation(void);
	float GetScale(void);
	int GetImageWidth(void);
	int GetImageHeight(void);

	void SetInputImageFileName(CString FileName);
	bool LoadImage(Bitmap** bitmap, CString *fileName);
	CLayerManager *m_layerManager;
	int m_imageWidth;
	int m_imageHeight;
	Magnification m_magnification;
private:
	int m_activeToolType;
public:
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnRButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedMovebutton();
	afx_msg void OnBnClickedZoombutton();
	afx_msg void OnBnClickedViewOriginalLayerButton();
	afx_msg void OnBnClickedViewVectorLayerButton();
private:
	int GetViewWidth(void);
	int GetViewHeight(void);
	void ZoomIn(const CPoint &a_point);
	void ZoomOut(const CPoint &a_point);
public:
	void SetImageCenter(PointF a_point);
private:
	float m_translationX;
	float m_translationY;
	void SetXTranslation(float a_Translation);
	void SetYTranslation(float a_Translation);
	AffineTransform GetTransform(void);
	bool m_mouseIsBeingDragged;
	CPoint m_previousDragPoint;
	void FillBackground(
		CDC *a_dc, 
		AffineTransform & l_transform, 
		int a_imageWidth, 
		int a_imageHeight);
public:
	BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
private:
	enum CursorTypes m_cursorType;
	void SetCursorType(enum CursorTypes a_cursorType);
	enum CursorTypes GetCursorType(void);
	bool AltKeyIsPressed(void);
public:
	void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnBnClickedViewAllLayersButton();
private:
	void UpdateLayerVisibilitiesFromToolbox(void);
	void ResetParameterSettings(void);
	void ResetView(void);
};

#ifndef _DEBUG  // debug version in LineTracerView.cpp
inline CLineTracerDoc* CLineTracerView::GetDocument() const
   { return reinterpret_cast<CLineTracerDoc*>(m_pDocument); }
#endif
