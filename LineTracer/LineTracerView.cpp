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
#include "Logger.h"
#include ".\linetracerview.h"

//#define USE_MEMDC 1

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

enum ToolTypes 
{ 
	ToolTypeNone,
	ToolTypeZoom,
	ToolTypeMove
};
enum CursorTypes 
{ 
	CursorTypeNone,
	CursorTypeZoomIn,
	CursorTypeZoomOut,
	CursorTypeWait,
	CursorTypeHand,
	CursorTypeClosedHand
};

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
	ON_UPDATE_COMMAND_UI(ID_VIEW_BEZIERMAKER, OnUpdateViewBeziermaker)
	ON_UPDATE_COMMAND_UI(ID_VIEW_THINNER, OnUpdateViewThinner)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_SETCURSOR()
	ON_BN_CLICKED(IDC_MOVEBUTTON, OnBnClickedMovebutton)
	ON_BN_CLICKED(IDC_ZOOMBUTTON, OnBnClickedZoombutton)
	ON_BN_CLICKED(IDC_VIEW_ORIGINAL_BUTTON, OnBnClickedViewOriginalLayerButton)
	ON_BN_CLICKED(IDC_VIEW_RESULT_BUTTON, OnBnClickedViewVectorLayerButton)
	ON_BN_CLICKED(IDC_VIEW_ALL_BUTTON, OnBnClickedViewAllLayersButton)
END_MESSAGE_MAP()


// CLineTracerView construction/destruction

CLineTracerView::CLineTracerView()
: m_activeToolType(ToolTypeZoom)
, m_translationX(0)
, m_translationY(0)
, m_mouseIsBeingDragged(false)
, m_previousDragPoint(0)
, m_cursorType(CursorTypeZoomIn)
{
	CLayerManager::Instance()->SetLineTracerView( this );
	CToolBox::Instance()->SetLineTracerView( this );
	m_layerManager = CLayerManager::Instance();
	m_imageWidth = 0;
	m_imageHeight = 0;
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
	/*SetScrollSizes(MM_TEXT, CSize( static_cast<int>(GetImageWidth()*GetScale()), 
		static_cast<int>(GetImageHeight()*GetScale())) );*/
	LOG ( "caught OnDraw\n" );

#ifdef	USE_MEMDC
    CMemDC pDC(dc);
#endif
	CLineTracerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	if ( GetImageWidth() == 0 )
	{
		//clear all background
		static const unsigned int l_BGCOLOR = 0x808080;
		CRect l_rect;
		dc->GetClipBox(l_rect);
		dc->FillSolidRect ( 
			CRect(0,0,l_rect.right,l_rect.bottom), 
			l_BGCOLOR );
		return;
	}
	
	AffineTransform l_transform = GetTransform();
	l_transform.Validate (
		CSize ( GetImageWidth(), GetImageHeight() ),
		CSize ( GetViewWidth(), GetViewHeight() )
		);
	m_translationX = l_transform.GetTranslationX();
	m_translationY = l_transform.GetTranslationY();

#ifdef	USE_MEMDC
	Graphics gr(pDC);
#else
	FillBackground(dc, l_transform, GetImageWidth(), GetImageHeight());
	Graphics gr(*dc);
#endif
	Matrix *l_matrix = l_transform.GetMatrix();
	(void) gr.SetTransform ( l_matrix );
	(void) gr.SetSmoothingMode ( SmoothingModeAntiAlias );
	(void) gr.SetInterpolationMode( InterpolationModeNearestNeighbor );

	CLayerManager *l_layerManager = CLayerManager::Instance();
	l_layerManager->DrawAllLayers ( gr );

	PostMessage ( WM_SETCURSOR, (WPARAM)this );

	delete l_matrix;
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
	TCHAR szFilters[] = _T("Images (*.jpg;*.gif;*.tiff;*.tif)");

	CFileDialog dlg (TRUE,_T("jpg;gif;tiff;tif"),
		_T("*.jpg;*.gif;*.tiff;*.tif"),OFN_FILEMUSTEXIST,szFilters);

	if(dlg.DoModal()==IDOK) 
	{
		ResetParameterSettings();
		SetInputImageFileName(dlg.GetPathName());
		ResetView();
		ProcessLayers();
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
#ifdef USE_MEMDC
	return FALSE;
#else
	return CView::OnEraseBkgnd(pDC);
#endif
}


void CLineTracerView::ProcessLayers(void)
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

float CLineTracerView::GetXTranslation(void)
{
	return m_translationX;
}

float CLineTracerView::GetYTranslation(void)
{
	return m_translationY;
}

float CLineTracerView::GetScale(void)
{
	return static_cast<float>(m_magnification.GetValue());
}

int CLineTracerView::GetImageWidth(void)
{
	return m_imageWidth;
}

int CLineTracerView::GetImageHeight(void)
{
	return m_imageHeight;
}

void CLineTracerView::SetInputImageFileName(CString FileName)
{
	CLineTracerDoc *l_document = GetDocument();
	ASSERT ( l_document != NULL );
	l_document->SetInputImageFileName(FileName);
	Bitmap *inputBitmap;

	if(FileName!=_T("")) {
		if(LoadImage(&inputBitmap, &FileName)) {
			CProjectSettings::Instance()->Init();

			CRawImage<ARGB> img(inputBitmap);

			CLayerManager *lm=CLayerManager::Instance();
			lm->InvalidateLayers();

			lm->GetLayer(CLayerManager::DESATURATOR)->Process(&img);
			lm->GetLayer(CLayerManager::DESATURATOR)->SetValid(true);
			delete inputBitmap;
		}
	}
}

bool CLineTracerView::LoadImage(Bitmap** image, CString *fileName)
{	
	LPWSTR lpszW = new WCHAR[1024];

	LPTSTR lpStr = fileName->GetBuffer(fileName->GetLength() );
	int nLen = MultiByteToWideChar(CP_ACP, 0,lpStr, -1, NULL, NULL);
	int l_status = MultiByteToWideChar(CP_ACP, 0, 	lpStr, -1, lpszW, nLen);
	ASSERT ( l_status != 0 );

	*image = new Bitmap(lpszW);

	delete[] lpszW;

	if ( (*image) != NULL )
	{
		m_imageWidth = (*image)->GetWidth();
		m_imageHeight = (*image)->GetHeight();
		return true;
	}
	else
	{
		return false;
	}
}

void CLineTracerView::OnLButtonDown(UINT nFlags, CPoint point)
{
	switch ( m_activeToolType )
	{
	case ToolTypeZoom:
		if ( AltKeyIsPressed() )
		{
			ZoomOut(point);
		}
		else 
		{
			ZoomIn(point);
		}
		break;
	case ToolTypeMove:
		SetCursorType(CursorTypeClosedHand);
		PostMessage(WM_SETCURSOR, (WPARAM)this);
		SetCapture();
		m_mouseIsBeingDragged = true;
		m_previousDragPoint = point;
		break;
	}
}

void CLineTracerView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	TRACE( "OnLButtonUp()\n" );
	if ( m_activeToolType == ToolTypeMove ) 
	{
		::ReleaseCapture();
		m_mouseIsBeingDragged = false;
		SetCursorType(CursorTypeHand);
	}
}

void CLineTracerView::OnRButtonDown(UINT nFlags, CPoint point)
{
	switch ( m_activeToolType )
	{
	case ToolTypeZoom:
		if ( AltKeyIsPressed() == false )
		{
			ZoomOut(point);
		}
		else 
		{
			ZoomIn(point);
		}
		break;
	}
}
void CLineTracerView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if ( m_activeToolType == ToolTypeMove ) 
	{
		if ( m_mouseIsBeingDragged ) 
		{
			CPoint l_difference = point - m_previousDragPoint;
			SetXTranslation ( l_difference.x + GetXTranslation() );
			SetYTranslation ( l_difference.y + GetYTranslation() );
			m_previousDragPoint = point;
			Invalidate(FALSE);
		}
	}
}

void CLineTracerView::OnBnClickedMovebutton()
{
	CToolBox::Instance()->MoveButtonClicked();
	m_activeToolType = ToolTypeMove;
	SetCursorType(CursorTypeHand);
}

void CLineTracerView::OnBnClickedZoombutton()
{
	SetCursorType(CursorTypeZoomIn);
	CToolBox::Instance()->ZoomButtonClicked();
	m_activeToolType = ToolTypeZoom;
}

void CLineTracerView::OnBnClickedViewOriginalLayerButton()
{
	CToolBox::Instance()->ViewOriginalLayerButtonClicked();
	UpdateLayerVisibilitiesFromToolbox();
}

void CLineTracerView::OnBnClickedViewVectorLayerButton()
{
	CToolBox::Instance()->ViewVectorLayerButtonClicked();
	UpdateLayerVisibilitiesFromToolbox();
}

void CLineTracerView::OnBnClickedViewAllLayersButton()
{
	CToolBox::Instance()->ViewAllLayersButtonClicked();
	UpdateLayerVisibilitiesFromToolbox();
}

int CLineTracerView::GetViewWidth(void)
{
	CRect l_clientRect;
	GetClientRect ( & l_clientRect );
	return l_clientRect.right;
}


int CLineTracerView::GetViewHeight(void)
{
	CRect l_clientRect;
	GetClientRect ( & l_clientRect );
	return l_clientRect.bottom;
}

void CLineTracerView::ZoomIn(const CPoint &a_point)
{
	AffineTransform l_inverse = GetTransform();
	l_inverse.Invert();

	PointF l_clickPoint ( (float)a_point.x, (float)a_point.y );
	l_inverse.TransformPoint(l_clickPoint);

	m_magnification.Increase();
	SetImageCenter(l_clickPoint);
	Invalidate(FALSE);
}

void CLineTracerView::ZoomOut(const CPoint &a_point)
{
	AffineTransform l_inverse = GetTransform();
	l_inverse.Invert();

	PointF l_clickPoint ( static_cast<float>(a_point.x), 
		static_cast<float>(a_point.y) );
	l_inverse.TransformPoint(l_clickPoint);

	m_magnification.Decrease();
	SetImageCenter(l_clickPoint);
	Invalidate(FALSE);
}

void CLineTracerView::SetImageCenter(PointF a_clickPoint)
{
	//reset translation
	SetXTranslation ( 0 );
	SetYTranslation ( 0 );

	GetTransform().TransformPoint( a_clickPoint );

	PointF l_viewCenter ( (float)GetViewWidth()/2,
		(float)GetViewHeight()/2 );

	//set new translation
	SetXTranslation ( l_viewCenter.X - a_clickPoint.X );
	SetYTranslation ( l_viewCenter.Y - a_clickPoint.Y );
}

void CLineTracerView::SetXTranslation(float a_translation)
{
	m_translationX = a_translation;
}

void CLineTracerView::SetYTranslation(float a_translation)
{
	m_translationY = a_translation;
}

AffineTransform CLineTracerView::GetTransform(void)
{
	AffineTransform l_transform;
	l_transform.TranslateBy(GetXTranslation(), GetYTranslation());
	l_transform.ScaleBy(GetScale());
	return l_transform;
}

void CLineTracerView::FillBackground(
	CDC *a_dc, 
	AffineTransform & a_transform, 
	int a_imageWidth, 
	int a_imageHeight)
{
	CRect l_rect;
	a_dc->GetClipBox(l_rect);

	PointF l_bottomRightImagePoint((float)a_imageWidth, (float)a_imageHeight);
	PointF l_topLeftImagePoint(0.0f, 0.0f);
	a_transform.TransformPoint(l_bottomRightImagePoint);
	a_transform.TransformPoint(l_topLeftImagePoint);

	/* areas
     _________________
	|    |  T    |    |
	|    |_______|    |
	|    |       |    |
	|    |image  |    |
	| L  |       | R  |
	|    |_______|    |
	|    |  B    |    |
	|____|_______|____|
	*/

	static const unsigned int l_BGCOLOR = 0x808080;
	//L
    a_dc->FillSolidRect ( 
		CRect(0,0,int(l_topLeftImagePoint.X)+1,l_rect.bottom), 
		l_BGCOLOR );
	//R
    a_dc->FillSolidRect ( 
		CRect(int(l_bottomRightImagePoint.X),
			0,
			l_rect.right,
			l_rect.bottom), 
		l_BGCOLOR );

	//T
    a_dc->FillSolidRect ( 
		CRect(int(l_topLeftImagePoint.X),0,
			int(l_bottomRightImagePoint.X)+1,int(l_topLeftImagePoint.Y)+1), 
		l_BGCOLOR );
	//B
    a_dc->FillSolidRect ( 
		CRect(int(l_topLeftImagePoint.X),int(l_bottomRightImagePoint.Y),
			int(l_bottomRightImagePoint.X)+1,l_rect.bottom), 
		l_BGCOLOR );

	/*
	//image background
	static const unsigned int l_WHITE = 0xffffff;
    a_dc->FillSolidRect ( 
		CRect(int(l_topLeftImagePoint.X), int(l_topLeftImagePoint.Y)+1, 
		int(l_bottomRightImagePoint.X)+1, int(l_bottomRightImagePoint.Y)),
		l_WHITE );
		*/
}



BOOL CLineTracerView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if ( CLayerManager::Instance()->IsProcessing() )
	{
		//show hourglass while working
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
		return TRUE;
	}

	bool l_messageFromAnotherWindow = ( pWnd == NULL );
	bool l_noImageLoaded = ( GetImageWidth() == 0 );

	if ( l_messageFromAnotherWindow || l_noImageLoaded )
	{
		//change to default arrow
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		return TRUE;
	}

	switch ( GetCursorType() )
	{
	case CursorTypeWait:
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
		return TRUE;
	case CursorTypeClosedHand:
		::SetCursor(AfxGetApp()->LoadCursor(IDC_CLOSED_HAND));
		return TRUE;
	case CursorTypeZoomIn:
		if ( AltKeyIsPressed() == false )
		{
			::SetCursor(AfxGetApp()->LoadCursor(IDC_ZOOM_IN));
		}
		else
		{
			::SetCursor(AfxGetApp()->LoadCursor(IDC_ZOOM_OUT));
		}
		return TRUE;
	case CursorTypeZoomOut:
		if ( AltKeyIsPressed() == false )
		{
			::SetCursor(AfxGetApp()->LoadCursor(IDC_ZOOM_OUT));
		}
		else
		{
			::SetCursor(AfxGetApp()->LoadCursor(IDC_ZOOM_IN));
		}
		return TRUE;
	case CursorTypeHand:
		::SetCursor(AfxGetApp()->LoadCursor(IDC_HAND));
		return TRUE;
	}
	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void CLineTracerView::SetCursorType(enum CursorTypes a_cursorType)
{
	m_cursorType = a_cursorType;
}

enum CursorTypes CLineTracerView::GetCursorType(void)
{
	return m_cursorType;
}

bool CLineTracerView::AltKeyIsPressed(void)
{
	return ( ::GetKeyState(VK_MENU) < 0 );
}

void CLineTracerView::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	TRACE ( "OnSysKeyDown\n" );
	if ( nChar == VK_MENU )
	{
		PostMessage ( WM_SETCURSOR, (WPARAM)this );
		return;
	}
	CView::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CLineTracerView::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ( nChar == VK_MENU )
	{
		PostMessage ( WM_SETCURSOR, (WPARAM)this );
		return;
	}
	CView::OnSysKeyUp(nChar, nRepCnt, nFlags);
}


void CLineTracerView::UpdateLayerVisibilitiesFromToolbox(void)
{
	bool l_originalLayerIsVisible = CToolBox::Instance()->IsOriginalLayerVisible();
	bool l_vectorLayerIsVisible = CToolBox::Instance()->IsVectorLayerVisible();

	CLayerManager::Instance()->SetOriginalLayerVisibility(l_originalLayerIsVisible);
	CLayerManager::Instance()->SetVectorLayerVisibility(l_vectorLayerIsVisible);

	Invalidate(FALSE);
}

void CLineTracerView::ResetParameterSettings(void)
{
	CProjectSettings::Instance()->Reset();
	BOOL l_result = ::PostMessage ( CToolBox::Instance()->m_hWnd, WM_UPDATE_TOOLBOX_DATA_FROM_LAYERS, 0, 0 );
	ASSERT ( l_result != 0 );
}

void CLineTracerView::ResetView(void)
{
	m_translationX = 0;
	m_translationY = 0;
	m_magnification.Reset();

	for (;;) 
	{
		bool l_viewWiderThanImage = ( GetViewWidth() > ( GetImageWidth() * GetScale() ) );
		bool l_viewHigherThanImage = ( GetViewHeight() > ( GetImageHeight() * GetScale() ) );

		if ( l_viewWiderThanImage && l_viewHigherThanImage )
		{
			break;
		}

		m_magnification.Decrease();
	}
}
