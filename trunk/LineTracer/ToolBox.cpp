// ToolBox.cpp : implementation file
//

#pragma once

#include "stdafx.h"
#include "LineTracer.h"
#include "ToolBox.h"

#include "LayerManager.h"
#include "Layer.h"

#include "LineTracerView.h"

// CToolBox

IMPLEMENT_DYNAMIC(CToolBox, CInitDialogBar)

BEGIN_MESSAGE_MAP(CToolBox, CInitDialogBar)
	ON_MESSAGE( (WM_UPDATE_TOOLBOX_DATA_FROM_LAYERS), (OnUpdateToolboxDataFromLayers) )
END_MESSAGE_MAP()

CToolBox::CToolBox()
: CInitDialogBar()
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

	c_spinButtonControl.SetRange(1,254);
	(void) c_spinButtonControl.SetPos(1);

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
	DDX_Control(pDX, IDC_SPIN1, c_spinButtonControl);
	//}}AFX_DATA_MAP
}

CToolBox* CToolBox::Instance(void)
{
    static CToolBox inst;
    return &inst;
}

void CToolBox::Init(void)
{
	//c_spinButtonControl.SetRange(1,254);
	/*
	CWnd *l_spinWnd = GetDlgItem(IDC_SPIN1);
	LOG ( "l_spinWnd: %x", l_spinWnd );
	ASSERT ( l_spinWnd != NULL );
	CSpinButtonCtrl *spinPtr = static_cast<CSpinButtonCtrl*>(l_spinWnd);
	ASSERT ( spinPtr != NULL );
	spinPtr->SetRange(1,254);*/
	//c_spinButtonControl.SetRange(1,254);
}

double CToolBox::GetParam(int type) const
{
	CLogger::Activate();

	LOG ( "CToolBox::GetParam(type) ; type == %i\n", type);
	LOG ( "CToolBox::BINARIZER == %i\n", BINARIZER);
	
	double retVal = -1.0;

	switch(type) {
		case BINARIZER:
			retVal = c_spinButtonControl.GetPos();
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
	CLogger::Activate();
	LOG ( "OnUpdateToolboxDataFromLayers\n" );

	CLayerManager *lm = CLayerManager::Instance();
	CLayer *l = lm->GetLayer(CLayerManager::BINARIZER);

	int l_paramVal = int( l->GetParam( CImageProcessor::BINARIZER_THRESHOLD ) );
	(void) c_spinButtonControl.SetPos( l_paramVal );

	return 0;	
}

