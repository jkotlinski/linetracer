// LineTracerView.cpp : implementation of the CLineTracerView class
//

#include "stdafx.h"
#include "LineTracer.h"
#include "FloatComparer.h"
#include <afxdlgs.h>

#include "LineTracerDoc.h"
#include "LineTracerView.h"
#include "EpsWriter.h"

#include "ToolBox.h"
#include "Binarizer.h"
#include "Logger.h"
#include "linetracerview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLineTracerView

IMPLEMENT_DYNCREATE(CLineTracerView, CScrollView)

BEGIN_MESSAGE_MAP(CLineTracerView, CScrollView)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_FILE_OPENIMAGE, OnFileOpenimage)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SKELETONIZER, OnUpdateViewSkeletonizer)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BINARIZER, OnUpdateViewBinarizer)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GAUSSIAN, OnUpdateViewGaussian)
	ON_COMMAND(ID_FILE_EXPORTEPS, OnFileExporteps)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ORIGINAL, OnUpdateViewOriginal)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_200, OnUpdateZoom200)
	ON_UPDATE_COMMAND_UI(ID_ZOOM_100, OnUpdateZoom100)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BEZIERMAKER, OnUpdateViewBeziermaker)
	ON_UPDATE_COMMAND_UI(ID_VIEW_THINNER, OnUpdateViewThinner)
END_MESSAGE_MAP()

// CLineTracerView construction/destruction

CLineTracerView::CLineTracerView()
{
	CLayerManager::Instance()->SetLineTracerView( this );
	CToolBox::Instance()->SetLineTracerView( this );
}

CLineTracerView::~CLineTracerView()
{
}

BOOL CLineTracerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

// CLineTracerView drawing

void CLineTracerView::OnDraw(CDC* dc)
{
	LOG ( "caught OnDraw\n" );

    CMemDC pDC(dc);
	CLineTracerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here

	CString inputFileName=pDoc->GetInputImageFileName();

	inputFileName.ReleaseBuffer();

	CLayerManager *lm = CLayerManager::Instance();

	//LOG("last layer valid: %x\n",lm->GetLastLayer()->IsValid());
	//if(lm->GetLastLayer()->IsValid()) {
		Bitmap *l_bmp = lm->GetBitmap();

		if ( l_bmp != NULL ) {
			Graphics gr(pDC);
			Status l_result = gr.SetInterpolationMode(InterpolationModeNearestNeighbor);
			ASSERT ( l_result == Ok );

			LOG( "l_bmp: %x\n", l_bmp );
			SetScrollSizes(MM_TEXT, CSize( l_bmp->GetWidth()*pDoc->GetZoom()/100, 
				l_bmp->GetHeight()*pDoc->GetZoom()/100));

			Status l_drawImageResult = gr.DrawImage(l_bmp,0,0,
				l_bmp->GetWidth()*pDoc->GetZoom()/100,
				l_bmp->GetHeight()*pDoc->GetZoom()/100);
			ASSERT ( l_drawImageResult == Ok );
		}
	//}
}


// CLineTracerView diagnostics

#ifdef _DEBUG
void CLineTracerView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CLineTracerView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CLineTracerDoc* CLineTracerView::GetDocument() const // non-debug version is inline
{
	return dynamic_cast<CLineTracerDoc*>(m_pDocument);
}
#endif //_DEBUG


// CLineTracerView message handlers

void CLineTracerView::OnFileOpenimage()
{
	TCHAR szFilters[] = _T("Images (*.jpg;*.gif;*.tiff)");

	CFileDialog dlg (TRUE,_T("jpg;gif;tiff"),
		_T("*.jpg;*.gif;*.tiff"),OFN_FILEMUSTEXIST,szFilters);

	if(dlg.DoModal()==IDOK) {
		CLineTracerDoc *pDoc = GetDocument();
		pDoc->SetInputImageFileName(dlg.GetPathName());
		pDoc->ProcessLayers();
	}
}


void CLineTracerView::OnInitialUpdate(void)
{
	CScrollView::OnInitialUpdate();

	SetScrollSizes(MM_TEXT, CSize(0, 0));
}


void CLineTracerView::OnUpdateViewSkeletonizer(CCmdUI *pCmdUI)
{
	if(CLayerManager::Instance()->LayerCount() > CLayerManager::SKELETONIZER) {
		CLayer* l = CLayerManager::Instance()->GetLayer(CLayerManager::SKELETONIZER);
		pCmdUI->SetCheck(l->IsVisible());
	}
}

void CLineTracerView::OnUpdateViewBinarizer(CCmdUI *pCmdUI)
{
	CLayer* l = CLayerManager::Instance()->GetLayer(CLayerManager::BINARIZER);
	pCmdUI->SetCheck(l->IsVisible());
}

void CLineTracerView::OnUpdateViewGaussian(CCmdUI *pCmdUI)
{
	CLayerManager *lm = CLayerManager::Instance();
	CLayer* l = lm->GetLayer(CLayerManager::GAUSSIAN);
	pCmdUI->SetCheck(l->IsVisible());
}

void CLineTracerView::OnFileExporteps() {
	TCHAR szFilters[] = _T("Encapsulated PostScript (*.eps)");

	CFileDialog dlg (FALSE,_T("eps"),
		_T("*.eps"),OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,szFilters);

	if(dlg.DoModal()==IDOK) {
		const CString l_pathName = dlg.GetPathName();
		CEpsWriter::Write(l_pathName);
	}
}


void CLineTracerView::OnUpdateViewOriginal(CCmdUI *pCmdUI)
{
	CLayerManager *lm = CLayerManager::Instance();
	CLayer* l = lm->GetLayer(CLayerManager::DESATURATOR);
	pCmdUI->SetCheck(l->IsVisible());
}

void CLineTracerView::OnUpdateZoom200(CCmdUI *pCmdUI)
{
	CLineTracerDoc *pDoc = GetDocument();
	pCmdUI->SetCheck( (pDoc->GetZoom()==200)?1:0 );
}

void CLineTracerView::OnUpdateZoom100(CCmdUI *pCmdUI)
{
	CLineTracerDoc *pDoc = GetDocument();
	pCmdUI->SetCheck( (pDoc->GetZoom()==100)?1:0 );
}

void CLineTracerView::OnUpdateViewBeziermaker(CCmdUI *pCmdUI)
{
	CLayer* l = CLayerManager::Instance()->GetLayer(CLayerManager::BEZIERMAKER);
	pCmdUI->SetCheck(l->IsVisible());
}

void CLineTracerView::OnUpdateViewThinner(CCmdUI *pCmdUI)
{
	CLayer* l = CLayerManager::Instance()->GetLayer(CLayerManager::THINNER);
	pCmdUI->SetCheck(l->IsVisible());
}

//lint -e{715}
afx_msg BOOL CLineTracerView::OnEraseBkgnd(CDC* pDC)
{
	LOG ( "OnEraseBkgnd()\n" );
	return FALSE;
}


void CLineTracerView::ProcessLayers(void) const
{
	CLayerManager *lm = CLayerManager::Instance();
	if(lm->GetLayer(0)->GetSketchImage() == NULL) return;

	CLayerManager::Instance()->ProcessLayers();
}

void CLineTracerView::HandleChangedToolboxParam(CLayerManager::LayerTypes a_layerId,
												CProjectSettings::ParamName a_paramName
												)
{
	LOG ( "HandleChangedToolboxParam; paramname=%i\n", a_paramName );
	CToolBox *l_toolBox = CToolBox::Instance();
	double l_sliderVal = l_toolBox->GetParam(a_paramName);

	CProjectSettings *l_settings = CProjectSettings::Instance();
	double l_layerVal = l_settings->GetParam(a_paramName);

	if( CFloatComparer::FloatsDiffer ( l_sliderVal, l_layerVal ) ) {
		LOG ( "FloatsDiffer\n" );
		l_settings->SetParam(a_paramName, l_sliderVal);
		CLayerManager::Instance()->InvalidateLayers(a_layerId);
		ProcessLayers();		
		GetDocument()->SetModifiedFlag();
	}
}

