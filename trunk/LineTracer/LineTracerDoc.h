// LineTracerDoc.h : interface of the CLineTracerDoc class
//

#pragma once

#include "LayerManager.h"

using namespace std;

class CLineTracerDoc : public CDocument
{
protected: // create from serialization only
	CLineTracerDoc();
	DECLARE_DYNCREATE(CLineTracerDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~CLineTracerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	CProjectSettings * GetProjectSettings();
private:
	CString m_InputBitmapFileName;
public:
	void ProcessLayers(void);

	afx_msg void OnViewSkeletonizer();
	afx_msg void OnViewBinarizer();
	afx_msg void OnViewOriginal();
	afx_msg void OnParametersLineLength();
	void SetZoom(int factor);
	int GetZoom(void) const;
private:
	int m_ZoomFactor;
	CProjectSettings m_project_settings;
public:
	afx_msg void OnViewBeziermaker();
	afx_msg void OnViewThinner();
	void SetInputImageFileName(CString & a_fileName);
};


