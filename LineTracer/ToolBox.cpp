// ToolBox.cpp : implementation file
//

#pragma once

#include "stdafx.h"
#include "LineTracer.h"
#include "ToolBox.h"
#include "ProjectSettings.h"

#include "LayerManager.h"
#include "ImageProcessor.h"
#include "Layer.h"
#include "LineTracerView.h"
#include ".\toolbox.h"

// CToolBox

IMPLEMENT_DYNAMIC(CToolBox, CInitDialogBar)

BEGIN_MESSAGE_MAP(CToolBox, CInitDialogBar)
	ON_MESSAGE( (WM_UPDATE_TOOLBOX_DATA_FROM_LAYERS), (OnUpdateToolboxDataFromLayers) )
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_HOLEFILLERSLIDER, OnDrawHoleFillerSlider)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_DETAILENCHANCESLIDER, OnNMReleasedCaptureDetailEnchanceSlider)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_TAILPRUNERSLIDER, OnLineLengthSlider)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_CURVEDETAILSLIDER, OnCurveDetailSlider)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_BWTHRESHOLD, OnToolboxChangeBwthreshold)
END_MESSAGE_MAP()

CToolBox::CToolBox()
: CInitDialogBar()
, m_lineTracerView(NULL)
, m_isInitialized(false)
, m_viewOriginalLayerButtonIsDown(true)
, m_viewVectorLayerButtonIsDown(true)
{
}

CToolBox::~CToolBox()
{
}

// CToolBox message handlers

BOOL CToolBox::OnInitDialogBar()
{
	// Support for DDX mechanism
	// If you do not want DDX then
	// do not call base class
	CInitDialogBar::OnInitDialogBar();

	// Update any controls NOT supported by DDX
	// CBitmapButton is one
	//m_OKButtton.AutoLoad(IDOK, this);

	CProjectSettings *l_settings = CProjectSettings::Instance();

	c_binarizerThresholdControl.SetRange(1,254);
	//(void) c_binarizerThresholdControl.SetPos(1);

	c_detailEnchanceControl.SetRange(1,20);
	c_detailEnchanceControl.SetPos( 
		21 - int( l_settings->GetParam( CProjectSettings::BINARIZER_MEAN_C ) ) );

	c_holeFillerControl.SetRange(0,40);
	c_holeFillerControl.SetPos( 
		int( l_settings->GetParam( CProjectSettings::HOLEFILLER_MAX_AREA_TO_FILL ) ) );

	c_tailPrunerControl.SetRange(0,10);
	c_tailPrunerControl.SetPos( 
		int( l_settings->GetParam( CProjectSettings::TAILPRUNER_THRESHOLD ) ) );

	c_curveDetailControl.SetRange(5,40);
	c_curveDetailControl.SetPos( 
		int( l_settings->GetParam( CProjectSettings::BEZIERMAKER_ERROR_THRESHOLD ) ) );

	c_zoomButton.SetState( TRUE );

	(void) c_zoomButton.SetIcon ( AfxGetApp()->LoadIcon(IDI_ZOOM) );
	(void) c_moveButton.SetIcon ( AfxGetApp()->LoadIcon(IDI_MOVE) );

	(void) c_viewAllLayersButton.SetIcon ( AfxGetApp()->LoadIcon(IDI_ALL_LAYERS) );
	(void) c_viewOriginalLayerButton.SetIcon ( AfxGetApp()->LoadIcon(IDI_ORIGINAL_LAYER) );
	(void) c_viewVectorLayerButton.SetIcon ( AfxGetApp()->LoadIcon(IDI_VECTOR_LAYER) );

	ViewAllLayersButtonClicked();

	m_isInitialized = true;
	return TRUE;
}

void CToolBox::DoDataExchange(CDataExchange* pDX)
{
	LOG ( "CToolBox::DoDataExchange()\n" );
	ASSERT ( pDX != NULL );

	CInitDialogBar::DoDataExchange(pDX);

	// DDX_??? functions to associate control with
	// data or control object
	// Call UpdateData(TRUE) to get data at any time
	// Call UpdateData(FALSE) to set data at any time

	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_BWTHRESHOLD, c_binarizerThresholdControl);
	DDX_Control(pDX, IDC_DETAILENCHANCESLIDER, c_detailEnchanceControl);
	DDX_Control(pDX, IDC_HOLEFILLERSLIDER, c_holeFillerControl);
	DDX_Control(pDX, IDC_TAILPRUNERSLIDER, c_tailPrunerControl);
	DDX_Control(pDX, IDC_CURVEDETAILSLIDER, c_curveDetailControl);
	DDX_Control(pDX, IDC_MOVEBUTTON, c_moveButton);
	DDX_Control(pDX, IDC_ZOOMBUTTON, c_zoomButton);
	DDX_Control(pDX, IDC_VIEW_RESULT_BUTTON, c_viewVectorLayerButton);
	DDX_Control(pDX, IDC_VIEW_ORIGINAL_BUTTON, c_viewOriginalLayerButton);
	DDX_Control(pDX, IDC_VIEW_ALL_BUTTON, c_viewAllLayersButton);
	//}}AFX_DATA_MAP
}

CToolBox* CToolBox::Instance(void)
{
    static CToolBox inst;
    return &inst;
}

double CToolBox::GetParam(CProjectSettings::ParamName a_name) const
{
	if ( m_isInitialized == false ) {
		return -1.0;
	}
	CLogger::Activate();

	double retVal = -1.0;

	switch( a_name ) {
		case CProjectSettings::BINARIZER_THRESHOLD:
			retVal = 255 - c_binarizerThresholdControl.GetPos();
			break;

		case CProjectSettings::BINARIZER_MEAN_C:
			retVal = 21 - c_detailEnchanceControl.GetPos();
			break;

		case CProjectSettings::HOLEFILLER_MAX_AREA_TO_FILL:
			retVal = c_holeFillerControl.GetPos();
			break;

		case CProjectSettings::TAILPRUNER_THRESHOLD:
			retVal = c_tailPrunerControl.GetPos();
			break;

		case CProjectSettings::BEZIERMAKER_ERROR_THRESHOLD:
			retVal = c_curveDetailControl.GetPos();
			break;

		default:
			//do nothing
			break;
	}
	
	LOG ( "CToolBox::GetParam() retval == %f\n", retVal);

	return retVal;
}

// ---------------------------------------------------
//lint -e{715} wParam and lParam are left unused
afx_msg LRESULT CToolBox::OnUpdateToolboxDataFromLayers
	(WPARAM wParam, LPARAM lParam)
{
	CProjectSettings *l_settings = CProjectSettings::Instance();
	CLogger::Activate();
	LOG ( "OnUpdateToolboxDataFromLayers\n" );

	int l_binarizerThresholdVal = int( l_settings->GetParam( CProjectSettings::BINARIZER_THRESHOLD ) );
	(void) c_binarizerThresholdControl.SetPos( 255 - l_binarizerThresholdVal );
	
	int l_detailEnchanceVal = int( l_settings->GetParam( CProjectSettings::BINARIZER_MEAN_C) );
	(void) c_detailEnchanceControl.SetPos( 21 - l_detailEnchanceVal );

	int l_holeFillerVal = int( l_settings->GetParam( CProjectSettings::HOLEFILLER_MAX_AREA_TO_FILL) );
	(void) c_holeFillerControl.SetPos( l_holeFillerVal );

	int l_tailPrunerVal = int( l_settings->GetParam( CProjectSettings::TAILPRUNER_THRESHOLD ) );
	(void) c_tailPrunerControl.SetPos( l_tailPrunerVal );

	int l_curveDetailVal = int( l_settings->GetParam( CProjectSettings::BEZIERMAKER_ERROR_THRESHOLD ) );
	(void) c_curveDetailControl.SetPos( l_curveDetailVal );

	return 0;	
}


void CToolBox::OnDrawHoleFillerSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	CLogger::Activate();
	LOG ( "caught OnDrawHoleFillerSlider()\n" );

	ASSERT ( m_lineTracerView );
	m_lineTracerView->HandleChangedToolboxParam( CLayerManager::HOLEFILLER,
		CProjectSettings::HOLEFILLER_MAX_AREA_TO_FILL);
	*pResult = 0;
}

void CToolBox::SetLineTracerView(CLineTracerView* a_view)
{
	m_lineTracerView = a_view;
}

afx_msg void CToolBox::OnNMReleasedCaptureDetailEnchanceSlider(NMHDR *pNMHDR, LRESULT *pResult) {
	ASSERT ( m_lineTracerView );
	m_lineTracerView->HandleChangedToolboxParam(CLayerManager::BINARIZER,
		CProjectSettings::BINARIZER_MEAN_C);
	*pResult = 0;
}

afx_msg void CToolBox::OnToolboxChangeBwthreshold(NMHDR *pNMHDR, LRESULT *pResult) {
	ASSERT ( m_lineTracerView );
	m_lineTracerView->HandleChangedToolboxParam(CLayerManager::BINARIZER,
		CProjectSettings::BINARIZER_THRESHOLD);
}

afx_msg void CToolBox::OnLineLengthSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	ASSERT ( m_lineTracerView );
	m_lineTracerView->HandleChangedToolboxParam(CLayerManager::TAILPRUNER,
		CProjectSettings::TAILPRUNER_THRESHOLD);
	*pResult = 0;
}

void CToolBox::OnCurveDetailSlider(NMHDR *pNMHDR, LRESULT *pResult)
{
	ASSERT ( m_lineTracerView );
	m_lineTracerView->HandleChangedToolboxParam(CLayerManager::BEZIERMAKER,
		CProjectSettings::BEZIERMAKER_ERROR_THRESHOLD);
	*pResult = 0;
}

void CToolBox::MoveButtonClicked()
{
	TRACE ("movebuttonclicked");
	c_moveButton.SetState( TRUE );
	c_zoomButton.SetState( FALSE );
}

void CToolBox::ZoomButtonClicked()
{
	TRACE ("zoombuttonclicked");
	c_moveButton.SetState( FALSE );
	c_zoomButton.SetState( TRUE );
}

void CToolBox::ViewOriginalLayerButtonClicked()
{
	ResetLayerViewButtons();
	c_viewOriginalLayerButton.SetState( TRUE );
}

void CToolBox::ViewVectorLayerButtonClicked()
{
	ResetLayerViewButtons();
	c_viewVectorLayerButton.SetState( TRUE );
}

void CToolBox::ViewAllLayersButtonClicked(void)
{
	ResetLayerViewButtons();
	c_viewAllLayersButton.SetState( TRUE );
}

void CToolBox::ResetLayerViewButtons(void)
{
	c_viewVectorLayerButton.SetState( FALSE );
	c_viewOriginalLayerButton.SetState( FALSE );
	c_viewAllLayersButton.SetState( FALSE );
}

bool CToolBox::IsVectorLayerVisible(void)
{
	if ( c_viewAllLayersButton.GetState() ||
		c_viewVectorLayerButton.GetState() )
	{
		return true;
	}
	return false;
}

bool CToolBox::IsOriginalLayerVisible(void)
{
	if ( c_viewAllLayersButton.GetState() ||
		c_viewOriginalLayerButton.GetState() )
	{
		return true;
	}
	return false;
}
