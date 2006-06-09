// LineTracerDoc.cpp : implementation of the CLineTracerDoc class
//

#include "stdafx.h"
#include "LineTracer.h"
#include "FloatComparer.h"

#include "ParamDialog.h"

#include "LineTracerDoc.h"
#include "ToolBox.h"

#include "Binarizer.h"
#include "DeSaturator.h"
#include "Skeletonizer.h"
#include "TailPruner.h"
#include "BezierMaker.h"

#include "RawImage.h"

#include "Logger.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <map>
#include ".\linetracerdoc.h"

// CLineTracerDoc

IMPLEMENT_DYNCREATE(CLineTracerDoc, CDocument)

BEGIN_MESSAGE_MAP(CLineTracerDoc, CDocument)
	ON_COMMAND(ID_VIEW_SKELETONIZER, OnViewSkeletonizer)
	ON_COMMAND(ID_VIEW_BINARIZER, OnViewBinarizer)
	ON_COMMAND(ID_VIEW_ORIGINAL, OnViewOriginal)
	ON_COMMAND(ID_PARAMETERS_LINESIZETHRESHOLD, OnParametersLineLength)
	ON_COMMAND(ID_VIEW_BEZIERMAKER, OnViewBeziermaker)
	ON_COMMAND(ID_VIEW_THINNER, OnViewThinner)
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
		//storing code
		ar << m_InputBitmapFileName;
		//PENDING: add CProjectSettings support
		/*
		ar << CBinarizer::Instance()->GetParam(CImageProcessor::BINARIZER_THRESHOLD);
		ar << CTailPruner::Instance()->GetParam(CImageProcessor::TAILPRUNER_THRESHOLD);
		*/
		ar << m_ZoomFactor;
		CLayerManager::Instance()->Serialize(ar);
	}
	else
	{
		// load info
		CLayerManager *lm = CLayerManager::Instance();
		//lm->InvalidateLayers();

		CString tmpString;

		ar >> tmpString;
		SetInputImageFileName(tmpString);

		//PENDING: add ProjectSettings support
		/*
		ar >> tmp;
		CBinarizer::Instance()->SetParam(CImageProcessor::BINARIZER_THRESHOLD,tmp);
		ar >> tmp;
		CTailPruner::Instance()->SetParam(CImageProcessor::TAILPRUNER_THRESHOLD,tmp);
		*/
		ar >> m_ZoomFactor;

		lm->Serialize(ar);

		if(lm->GetLayer(CLayerManager::DESATURATOR)->IsValid()) {
			//ProcessLayers();
		}
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

void CLineTracerDoc::ProcessLayers(void)
{
	CLayerManager *lm = CLayerManager::Instance();
	if(lm->GetLayer(0)->GetSketchImage() == NULL) return;

	//CLayerManager::Instance()->ProcessLayers();

	LOG( "UpdateAllViews()\n" );
	UpdateAllViews(NULL);
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

	UpdateAllViews(NULL);
	SetModifiedFlag();
}

void CLineTracerDoc::OnViewOriginal()
{
	CLayerManager *lm = CLayerManager::Instance();
	//CLayer *l = lm->GetLayer(CLayerManager::HOLEFILLER);
	CLayer* l = lm->GetLayer(CLayerManager::BINARIZER);
	l->SetVisible(!l->IsVisible());

	UpdateAllViews(NULL);
	SetModifiedFlag();
}

void CLineTracerDoc::OnParametersLineLength()
{
	/*CParamDialog dlg;
	CProjectSettings *l_settings = CProjectSettings::Instance();

	double oldVal = l_settings->GetParam(CProjectSettings::TAILPRUNER_THRESHOLD);
	dlg.m_EditValue.Format("%.0f",oldVal);

	if(dlg.DoModal() == IDOK) {
		double newVal = atof((char*)(const char*)dlg.m_EditValue);

		l_settings->SetParam(CProjectSettings::TAILPRUNER_THRESHOLD,newVal);
		if( CFloatComparer::FloatsDiffer(newVal,oldVal) ) {
			SetModifiedFlag();
			//CLayerManager::Instance()->InvalidateLayers(CLayerManager::TAILPRUNER);
			//ProcessLayers();
		}
	}*/
}

void CLineTracerDoc::SetZoom(int factor)
{
	m_ZoomFactor=factor;
	UpdateAllViews(NULL);
}

int CLineTracerDoc::GetZoom(void) const
{
	return m_ZoomFactor;
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


void CLineTracerDoc::SetInputImageFileName(CString & a_fileName)
{
	m_InputBitmapFileName = a_fileName;
	SetModifiedFlag();
}

CProjectSettings * CLineTracerDoc::GetProjectSettings() {
	return &m_project_settings;
}
