// LineTracerDoc.cpp : implementation of the CLineTracerDoc class
//

#include "stdafx.h"
#include "LineTracer.h"

#include "ParamDialog.h"

#include "LineTracerDoc.h"
#include ".\linetracerdoc.h"

#include "Binarizer.h"
#include "DeColorizer.h"
#include "Gaussian.h"
#include "Skeletonizer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <hash_map>
using namespace stdext;

// CLineTracerDoc

IMPLEMENT_DYNCREATE(CLineTracerDoc, CDocument)

BEGIN_MESSAGE_MAP(CLineTracerDoc, CDocument)
	ON_COMMAND(ID_PARAMETERS_BINARIZE, OnParametersBinarizer)
	ON_COMMAND(ID_PARAMETERS_GAUSSIANBLUR, OnParametersGaussian)
	ON_COMMAND(ID_VIEW_SKELETONIZER, OnViewSkeletonizer)
	ON_COMMAND(ID_VIEW_BINARIZER, OnViewBinarizer)
	ON_COMMAND(ID_VIEW_GAUSSIAN, OnViewGaussian)
END_MESSAGE_MAP()


// CLineTracerDoc construction/destruction

CLineTracerDoc::CLineTracerDoc()
: m_InputBitmapFileName(_T(""))
, m_InputBitmap(NULL)
{
}

CLineTracerDoc::~CLineTracerDoc()
{
	if(m_InputBitmap!=NULL) delete m_InputBitmap;
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
		ar << CGaussian::Instance()->GetParam("radius");
		ar << CBinarizer::Instance()->GetParam("threshold");
	}
	else
	{
		// TODO: add loading code here
		CString tmpString;
		double tmp;
		ar >> tmpString;
		ar >> tmp;
		CGaussian::Instance()->SetParam("radius",tmp);
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
			CBinarizer::Instance()->SetParam("threshold",-1);

			CLayerManager::Instance()->InvalidateLayers();
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

	CLayerManager::Instance()->ProcessLayers(m_InputBitmap);
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


void CLineTracerDoc::OnParametersBinarizer()
{
	CParamDialog dlg;
	CBinarizer *binarizer = CBinarizer::Instance();

	double oldVal = binarizer->GetParam("threshold");
	dlg.m_EditValue.Format("%.0f",oldVal);

	if(dlg.DoModal() == IDOK) {
		double newVal = atof((char*)(const char*)dlg.m_EditValue);

		binarizer->SetParam("threshold",newVal);
		if(newVal!=oldVal) {
			SetModifiedFlag();
			CLayerManager::Instance()->InvalidateLayers(CLayerManager::BINARIZER);
			CLayerManager::Instance()->ProcessLayers(m_InputBitmap);
		}
	}
}

void CLineTracerDoc::OnParametersGaussian()
{
	CParamDialog dlg;
	CGaussian *gaussian = CGaussian::Instance();

	double oldVal = gaussian->GetParam("radius");
	dlg.m_EditValue.Format("%.1f",oldVal);

	if(dlg.DoModal() == IDOK) {
		double newVal = atof((char*)(const char*)dlg.m_EditValue);

		if(newVal<0.0) newVal=0.0;

		gaussian->SetParam("radius",newVal);
		if(newVal!=oldVal) {
			SetModifiedFlag();
			CBinarizer::Instance()->SetParam("threshold",-1);

			CLayerManager::Instance()->InvalidateLayers(CLayerManager::GAUSSIAN);
			ProcessLayers();
		}
	}
}


void CLineTracerDoc::OnViewSkeletonizer()
{
	CLayer* l = CLayerManager::Instance()->GetLayer(CLayerManager::SKELETONIZER);
	l->SetVisible(!l->IsVisible());
	UpdateAllViews(NULL);
}

void CLineTracerDoc::OnViewBinarizer()
{
	CLayer* l = CLayerManager::Instance()->GetLayer(CLayerManager::BINARIZER);
	l->SetVisible(!l->IsVisible());
	UpdateAllViews(NULL);
}

void CLineTracerDoc::OnViewGaussian()
{
	CLayer* l = CLayerManager::Instance()->GetLayer(CLayerManager::GAUSSIAN);
	l->SetVisible(!l->IsVisible());
	UpdateAllViews(NULL);
}
