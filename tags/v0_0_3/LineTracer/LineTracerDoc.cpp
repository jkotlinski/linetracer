// LineTracerDoc.cpp : implementation of the CLineTracerDoc class
//

#include "stdafx.h"
#include "LineTracer.h"

#include "ParamDialog.h"

#include "LineTracerDoc.h"
#include ".\linetracerdoc.h"

#include "Binarizer.h"
#include "DeSaturator.h"
#include "Gaussian.h"
#include "Skeletonizer.h"

#include "RawImage.h"

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
	ON_COMMAND(ID_VIEW_ORIGINAL, OnViewOriginal)
END_MESSAGE_MAP()


// CLineTracerDoc construction/destruction

CLineTracerDoc::CLineTracerDoc()
: m_InputBitmapFileName(_T(""))
{
}

CLineTracerDoc::~CLineTracerDoc()
{
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
		TRACE("<< %f\n",CGaussian::Instance()->GetParam("radius"));
		ar << CBinarizer::Instance()->GetParam("threshold");
		CLayerManager::Instance()->Serialize(ar);
	}
	else
	{
		// TODO: add loading code here
		CLayerManager *lm = CLayerManager::Instance();
		lm->InvalidateLayers();

		CString tmpString;
		double tmp;
		ar >> tmpString;
		SetInputImageFileName(tmpString);

		ar >> tmp;

		CGaussian::Instance()->SetParam("radius",tmp);

		ar >> tmp;
		CBinarizer::Instance()->SetParam("threshold",tmp);

		lm->Serialize(ar);

		if(lm->GetLayer(CLayerManager::DESATURATOR)->IsValid()) {
			lm->ProcessLayers();
		}
		TRACE(">> gauss: %f\n",CGaussian::Instance()->GetParam("radius"));
		TRACE(">> thresh: %f\n",CBinarizer::Instance()->GetParam("threshold"));
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
	m_InputBitmapFileName=FileName;
	Bitmap *inputBitmap;

	if(FileName!=_T("")) {
		if(LoadImage(&inputBitmap, &FileName)) {
			SetModifiedFlag();
			CBinarizer::Instance()->SetParam("threshold",-1);

			CRawImage img(inputBitmap);

			CLayerManager *lm=CLayerManager::Instance();
			lm->InvalidateLayers();

			lm->GetLayer(CLayerManager::DESATURATOR)->Process(&img);
			lm->GetLayer(CLayerManager::DESATURATOR)->SetValid(true);
			delete inputBitmap;
		}
	}
}

CString CLineTracerDoc::GetInputImageFileName(void)
{
	return m_InputBitmapFileName;
}

void CLineTracerDoc::ProcessLayers(void)
{
	CLayerManager *lm = CLayerManager::Instance();
	if(lm->GetLayer(0)->GetSketchImage() == NULL) return;

	CLayerManager::Instance()->ProcessLayers();
	UpdateAllViews(NULL);
}

bool CLineTracerDoc::LoadImage(Bitmap** image, CString *fileName)
{	
	LPWSTR lpszW = new WCHAR[1024];

	LPTSTR lpStr = fileName->GetBuffer(fileName->GetLength() );
	int nLen = MultiByteToWideChar(CP_ACP, 0,lpStr, -1, NULL, NULL);
	MultiByteToWideChar(CP_ACP, 0, 	lpStr, -1, lpszW, nLen);

	*image = new Bitmap(lpszW);

	delete[] lpszW;

	return (*image!=NULL)?true:false;
}

/*Bitmap* CLineTracerDoc::GetInputBitmap(void)
{
	return m_InputBitmap;
}*/


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
			ProcessLayers();
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
	SetModifiedFlag();
}

void CLineTracerDoc::OnViewBinarizer()
{
	CLayerManager *lm = CLayerManager::Instance();
	CLayer *l = lm->GetLayer(CLayerManager::BINARIZER);
	l->SetVisible(!l->IsVisible());

	lm->GetLayer(CLayerManager::DESATURATOR)->SetVisible(false);
	lm->GetLayer(CLayerManager::GAUSSIAN)->SetVisible(false);

	UpdateAllViews(NULL);
	SetModifiedFlag();
}

void CLineTracerDoc::OnViewGaussian()
{
	CLayerManager *lm = CLayerManager::Instance();
	CLayer *l = lm->GetLayer(CLayerManager::GAUSSIAN);
	l->SetVisible(!l->IsVisible());

	lm->GetLayer(CLayerManager::DESATURATOR)->SetVisible(false);
	lm->GetLayer(CLayerManager::BINARIZER)->SetVisible(false);

	UpdateAllViews(NULL);
	SetModifiedFlag();
}

void CLineTracerDoc::OnViewOriginal()
{
	CLayerManager *lm = CLayerManager::Instance();
	CLayer *l = lm->GetLayer(CLayerManager::DESATURATOR);
	l->SetVisible(!l->IsVisible());

	lm->GetLayer(CLayerManager::BINARIZER)->SetVisible(false);
	lm->GetLayer(CLayerManager::GAUSSIAN)->SetVisible(false);

	UpdateAllViews(NULL);
	SetModifiedFlag();
}
