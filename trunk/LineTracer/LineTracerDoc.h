// LineTracerDoc.h : interface of the CLineTracerDoc class
//

#pragma once

#include <vector>
#include "layer.h"

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
	CString GetInputImageFileName(void);
private:
	CString m_InputBitmapFileName;

	vector<CLayer*> m_Layers;

	Bitmap *m_InputBitmap;
	bool LoadImage(Bitmap** bitmap, CString *fileName);

public:
	void ProcessLayers(void);
	Bitmap* GetInputBitmap(void);
	CLayer* GetLayer(int layer);
	afx_msg void OnParametersBinarizer();
	afx_msg void OnParametersGaussian();
};


