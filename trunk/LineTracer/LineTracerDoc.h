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
	void SetInputImageFileName(CString FileName);
	CString GetInputImageFileName(void) const;
private:
	CString m_InputBitmapFileName;

	bool LoadImage(Bitmap** bitmap, CString *fileName) const;

public:
	void ProcessLayers(void);
	//Bitmap* GetInputBitmap(void);
	afx_msg void OnParametersBinarizer();
	afx_msg void OnParametersGaussian();

	afx_msg void OnViewSkeletonizer();
	afx_msg void OnViewBinarizer();
	afx_msg void OnViewGaussian();
	afx_msg void OnViewOriginal();
	afx_msg void OnParametersLineLength();
	afx_msg void OnZoom100();
	afx_msg void OnZoom200();
	void SetZoom(int factor);
	int GetZoom(void) const;
private:
	int m_ZoomFactor;
public:
	afx_msg void OnParametersNoisesurpression();
	afx_msg void OnViewBeziermaker();
	afx_msg void OnViewThinner();
	afx_msg void OnParametersCurvedetail();
private:
	static bool FloatsDiffer ( double a_val1, double a_val2 );
};


