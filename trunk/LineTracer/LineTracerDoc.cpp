// LineTracerDoc.cpp : implementation of the CLineTracerDoc class
//

#include "stdafx.h"
#include "LineTracer.h"

#include "ParamDialog.h"

#include "LineTracerDoc.h"
#include ".\linetracerdoc.h"

#include "Binarizer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <hash_map>
using namespace stdext;

// CLineTracerDoc

IMPLEMENT_DYNCREATE(CLineTracerDoc, CDocument)

BEGIN_MESSAGE_MAP(CLineTracerDoc, CDocument)
	ON_COMMAND(ID_PARAMETERS_BINARIZE, OnParametersBinarizer)
END_MESSAGE_MAP()


// CLineTracerDoc construction/destruction

CLineTracerDoc::CLineTracerDoc()
: m_InputBitmapFileName(_T(""))
, m_InputBitmap(NULL)
{
	// TODO: add one-time construction code here
	CLayer *layer=new CLayer();
	layer->SetImageProcessor(CBinarizer::Instance());
	m_Layers.push_back(layer);
}

CLineTracerDoc::~CLineTracerDoc()
{
	if(m_InputBitmap!=NULL) delete m_InputBitmap;

	CLayer *layer=m_Layers.at(0);
	m_Layers.pop_back();
	delete layer;
}

BOOL CLineTracerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	SetInputImageFileName(CString(_T("")));

	return TRUE;
}




// CLineTracerDoc serialization

void CLineTracerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		ar << m_InputBitmapFileName;
		ar << CBinarizer::Instance()->GetParam("threshold");
	}
	else
	{
		// TODO: add loading code here
		CString tmpString;
		double tmp;
		ar >> tmpString;
		ar >> tmp;
		CBinarizer::Instance()->SetParam("threshold",tmp);
		this->SetInputImageFileName(tmpString);
	}
}


// CLineTracerDoc diagnostics

#ifdef _DEBUG
void CLineTracerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLineTracerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CLineTracerDoc commands

void CLineTracerDoc::SetInputImageFileName(CString FileName)
{
	TRACE("m_InputBitmap: %x\n",m_InputBitmap);

	m_InputBitmapFileName=FileName;

	if(FileName!=_T("")) {
		if(LoadImage(&m_InputBitmap, &FileName)) {
			SetModifiedFlag();
			ProcessLayers();
		}
	}
}

CString CLineTracerDoc::GetInputImageFileName(void)
{
	return m_InputBitmapFileName;
}

void CLineTracerDoc::ProcessLayers(void)
{
	if(m_InputBitmap==NULL) return;

	CRawImage *currBmp = new CRawImage(m_InputBitmap);

	for(UINT i=0; i<m_Layers.size(); i++) {
		CLayer *layer = m_Layers.at(i);

		layer->Process(currBmp);
	}
	delete currBmp;
	UpdateAllViews(NULL);
}

bool CLineTracerDoc::LoadImage(Bitmap** image, CString *fileName)
{	
	if(*image!=NULL) delete *image;

	*image=NULL;

	LPWSTR lpszW = new WCHAR[1024];

	LPTSTR lpStr = fileName->GetBuffer(fileName->GetLength() );
	int nLen = MultiByteToWideChar(CP_ACP, 0,lpStr, -1, NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, 	lpStr, -1, lpszW, nLen);

	*image = new Bitmap(lpszW);

	delete[] lpszW;

	return (*image!=NULL)?true:false;
}

Bitmap* CLineTracerDoc::GetInputBitmap(void)
{
	return m_InputBitmap;
}

CLayer* CLineTracerDoc::GetLayer(int layer)
{
	return m_Layers.at(layer);
}


void CLineTracerDoc::OnParametersBinarizer()
{
	CParamDialog dlg;
	CBinarizer *binarizer = CBinarizer::Instance();

	double oldVal = binarizer->GetParam("threshold");
	dlg.m_EditValue.Format("%.1f",oldVal);

	if(dlg.DoModal() == IDOK) {
		double newVal = atof((char*)(const char*)dlg.m_EditValue);

		binarizer->SetParam("threshold",newVal);
		if(newVal!=oldVal) {
			SetModifiedFlag();
			ProcessLayers();
		}
	}

	//binarizer->SetParam("threshold"
}
