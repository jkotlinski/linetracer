// LineTracerView.cpp : implementation of the CLineTracerView class
//

#include "stdafx.h"
#include "LineTracer.h"

#include <afxdlgs.h>

#include "LineTracerDoc.h"
#include "LineTracerView.h"
#include ".\linetracerview.h"
#include "EpsWriter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLineTracerView

IMPLEMENT_DYNCREATE(CLineTracerView, CScrollView)

BEGIN_MESSAGE_MAP(CLineTracerView, CScrollView)
	ON_COMMAND(ID_FILE_OPENIMAGE, OnFileOpenimage)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SKELETONIZER, OnUpdateViewSkeletonizer)
	ON_UPDATE_COMMAND_UI(ID_VIEW_BINARIZER, OnUpdateViewBinarizer)
	ON_UPDATE_COMMAND_UI(ID_VIEW_GAUSSIAN, OnUpdateViewGaussian)
	ON_COMMAND(ID_FILE_EXPORTEPS, OnFileExporteps)
END_MESSAGE_MAP()

// CLineTracerView construction/destruction

CLineTracerView::CLineTracerView()
{
	// TODO: add construction code here
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

void CLineTracerView::OnDraw(CDC* pDC)
{
	CLineTracerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here

	CString inputFileName=pDoc->GetInputImageFileName();

	inputFileName.ReleaseBuffer();

	if(pDoc->GetInputBitmap()!=NULL) {
		Bitmap *b=CLayerManager::Instance()->MakeBitmap();

		Graphics gr(*pDC);
		Bitmap *original=pDoc->GetInputBitmap();
		gr.DrawImage(original,0,0,original->GetWidth(),original->GetHeight());

		SetScrollSizes(MM_TEXT, CSize(b->GetWidth(), b->GetHeight()));

		gr.DrawImage(b,0,0);
		delete b;
	}

	/*
	CBrush blackBrush;
	blackBrush.CreateSolidBrush(RGB(0,0,0));
	*/
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
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLineTracerDoc)));
	return (CLineTracerDoc*)m_pDocument;
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
	CLayer* l = CLayerManager::Instance()->GetLayer(CLayerManager::SKELETONIZER);
	pCmdUI->SetCheck(l->IsVisible());
}

void CLineTracerView::OnUpdateViewBinarizer(CCmdUI *pCmdUI)
{
	CLayer* l = CLayerManager::Instance()->GetLayer(CLayerManager::BINARIZER);
	pCmdUI->SetCheck(l->IsVisible());
}

void CLineTracerView::OnUpdateViewGaussian(CCmdUI *pCmdUI)
{
	CLayer* l = CLayerManager::Instance()->GetLayer(CLayerManager::GAUSSIAN);
	pCmdUI->SetCheck(l->IsVisible());
}

void CLineTracerView::OnFileExporteps() {
	TCHAR szFilters[] = _T("Encapsulated PostScript (*.eps)");

	CFileDialog dlg (FALSE,_T("eps"),
		_T("*.eps"),OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,szFilters);

	if(dlg.DoModal()==IDOK) {
		CEpsWriter::Write(&dlg.GetPathName());
	}
}
