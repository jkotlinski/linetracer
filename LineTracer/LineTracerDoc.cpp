// LineTracerDoc.cpp : implementation of the CLineTracerDoc class
//

#include "stdafx.h"
#include "LineTracer.h"

#include "ParamDialog.h"

#include "LineTracerDoc.h"
#include "ToolBox.h"

#include "Binarizer.h"
#include "DeSaturator.h"
#include "Gaussian.h"
#include "Skeletonizer.h"
#include "TailPruner.h"
#include "BezierMaker.h"

#include "RawImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <map>

// CLineTracerDoc

IMPLEMENT_DYNCREATE(CLineTracerDoc, CDocument)

BEGIN_MESSAGE_MAP(CLineTracerDoc, CDocument)
	ON_COMMAND(ID_PARAMETERS_BINARIZE, OnParametersBinarizer)
	ON_COMMAND(ID_PARAMETERS_GAUSSIANBLUR, OnParametersGaussian)
	ON_COMMAND(ID_VIEW_SKELETONIZER, OnViewSkeletonizer)
	ON_COMMAND(ID_VIEW_BINARIZER, OnViewBinarizer)
	ON_COMMAND(ID_VIEW_GAUSSIAN, OnViewGaussian)
	ON_COMMAND(ID_VIEW_ORIGINAL, OnViewOriginal)
	ON_COMMAND(ID_PARAMETERS_LINESIZETHRESHOLD, OnParametersLineLength)
	ON_COMMAND(ID_ZOOM_100, OnZoom100)
	ON_COMMAND(ID_ZOOM_200, OnZoom200)
	ON_COMMAND(ID_PARAMETERS_NOISESURPRESSION, OnParametersNoisesurpression)
	ON_COMMAND(ID_VIEW_BEZIERMAKER, OnViewBeziermaker)
	ON_COMMAND(ID_VIEW_THINNER, OnViewThinner)
	ON_COMMAND(ID_PARAMETERS_CURVEDETAIL, OnParametersCurvedetail)
	ON_EN_CHANGE(IDC_BWTHRESHOLD, OnToolboxChangeBwthreshold)
END_MESSAGE_MAP()


// CLineTracerDoc construction/destruction

CLineTracerDoc::CLineTracerDoc()
: m_InputBitmapFileName(_T(""))
, m_ZoomFactor(100)
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
	SetZoom(100);

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
		ar << CTailPruner::Instance()->GetParam("threshold");
		ar << m_ZoomFactor;
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
		ar >> tmp;
		CTailPruner::Instance()->SetParam("threshold",tmp);
		ar >> m_ZoomFactor;

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
			CBinarizer::Instance()->SetParam("min_threshold",-1);

			CRawImage<ARGB> img(inputBitmap);

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

	double oldVal = binarizer->GetParam("min_threshold");
	dlg.m_EditValue.Format("%.0f",oldVal);

	if(dlg.DoModal() == IDOK) {
		double newVal = atof((char*)(const char*)dlg.m_EditValue);

		binarizer->SetParam("min_threshold",newVal);
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
			CBinarizer::Instance()->SetParam("min_threshold",-1);

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

void CLineTracerDoc::OnParametersLineLength()
{
	CParamDialog dlg;
	CTailPruner *pruner = CTailPruner::Instance();

	double oldVal = pruner->GetParam("threshold");
	dlg.m_EditValue.Format("%.0f",oldVal);

	if(dlg.DoModal() == IDOK) {
		double newVal = atof((char*)(const char*)dlg.m_EditValue);

		pruner->SetParam("threshold",newVal);
		if(newVal!=oldVal) {
			SetModifiedFlag();
			CLayerManager::Instance()->InvalidateLayers(CLayerManager::TAILPRUNER);
			ProcessLayers();
		}
	}
}

void CLineTracerDoc::OnZoom100()
{
	SetZoom(100);
}

void CLineTracerDoc::OnZoom200()
{
	SetZoom(200);
}

void CLineTracerDoc::SetZoom(int factor)
{
	m_ZoomFactor=factor;
	UpdateAllViews(NULL);
}

int CLineTracerDoc::GetZoom(void)
{
	return m_ZoomFactor;
}

void CLineTracerDoc::OnParametersNoisesurpression()
{
	CParamDialog dlg;
	CBinarizer *binarizer = CBinarizer::Instance();

	double oldVal = binarizer->GetParam("mean_c");
	dlg.m_EditValue.Format("%.0f",oldVal);

	if(dlg.DoModal() == IDOK) {
		double newVal = atof((char*)(const char*)dlg.m_EditValue);

		binarizer->SetParam("mean_c",newVal);
		if(newVal!=oldVal) {
			SetModifiedFlag();
			CLayerManager::Instance()->InvalidateLayers(CLayerManager::BINARIZER);
			ProcessLayers();
		}
	}
}

void CLineTracerDoc::OnViewBeziermaker()
{
	CLayer* l = CLayerManager::Instance()->GetLayer(CLayerManager::BEZIERMAKER);
	l->SetVisible(!l->IsVisible());
	UpdateAllViews(NULL);
	SetModifiedFlag();
}

void CLineTracerDoc::OnViewThinner()
{
	CLayer* l = CLayerManager::Instance()->GetLayer(CLayerManager::THINNER);
	l->SetVisible(!l->IsVisible());
	UpdateAllViews(NULL);
	SetModifiedFlag();
}

void CLineTracerDoc::OnParametersCurvedetail()
{
	CParamDialog dlg;
	CBezierMaker *bezierMaker = CBezierMaker::Instance();

	double oldVal = bezierMaker->GetParam("error_threshold");
	dlg.m_EditValue.Format("%.1f",oldVal);

	if(dlg.DoModal() == IDOK) {
		double newVal = atof((char*)(const char*)dlg.m_EditValue);

		if(newVal<10) newVal=10;
		bezierMaker->SetParam("error_threshold",newVal);
		if(newVal!=oldVal) {
			SetModifiedFlag();
			CLayerManager::Instance()->InvalidateLayers(CLayerManager::BEZIERMAKER);
			ProcessLayers();
		}
	}
}

void CLineTracerDoc::OnToolboxChangeBwthreshold()
{
	CToolBox *tb = CToolBox::Instance();
	
	double newVal = tb->GetParam(CToolBox::BINARIZER);

	CBinarizer *binarizer = CBinarizer::Instance();

	double oldVal = binarizer->GetParam("min_threshold");

	binarizer->SetParam("min_threshold",newVal);
	if(newVal!=oldVal) {
		SetModifiedFlag();
		CLayerManager::Instance()->InvalidateLayers(CLayerManager::BINARIZER);
		ProcessLayers();
	}
}
