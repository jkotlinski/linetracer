// ToolBox.cpp : implementation file
//

#include "stdafx.h"
#include "LineTracer.h"
#include "ToolBox.h"
#include ".\toolbox.h"

#include "LayerManager.h"
#include "Layer.h"

// CToolBox

IMPLEMENT_DYNAMIC(CToolBox, CDialogBar)
CToolBox::CToolBox()
{
}

CToolBox::~CToolBox()
{
}


BEGIN_MESSAGE_MAP(CToolBox, CDialogBar)
END_MESSAGE_MAP()



// CToolBox message handlers


CToolBox* CToolBox::Instance(void)
{
    static CToolBox inst;
    return &inst;
}

void CToolBox::Init(void)
{
	CSpinButtonCtrl *spinPtr = static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN1));

	spinPtr->SetRange(1,254);
}

void CToolBox::UpdateParams(void)
{
	CLayerManager *lm = CLayerManager::Instance();
	CLayer *l;

	l = lm->GetLayer(CLayerManager::BINARIZER);

	double bwThreshold = l->GetParam("min_threshold");

	//black-white-threshold
	CSpinButtonCtrl *spinPtr = static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN1));
	spinPtr->SetPos(int(bwThreshold));
}

double CToolBox::GetParam(int type)
{
	double retVal = -1;

	switch(type) {
		case BINARIZER:
			CSpinButtonCtrl *spinPtr = static_cast<CSpinButtonCtrl*>(GetDlgItem(IDC_SPIN1));
			retVal = spinPtr->GetPos();
			break;
	}
	
	return retVal;
}
