#pragma once

#include "LayerManager.h"
#include "InitDialogBar.h"

// CToolBox

class CToolBox : public CInitDialogBar
{
	DECLARE_DYNAMIC(CToolBox)

public:
	static CToolBox* Instance(void);

private:
	//lint -e{1704} singleton
	CToolBox();
	virtual ~CToolBox();

protected:
	CSpinButtonCtrl c_spinButtonControl;

public:
	void Init(void);

	enum ParamType { BINARIZER };
	double GetParam(int type) const;

protected:
	virtual void DoDataExchange(CDataExchange *pDX);
	virtual BOOL OnInitDialogBar();
	afx_msg LRESULT OnUpdateToolboxDataFromLayers ( WPARAM wParam, LPARAM lParam );
	
	DECLARE_MESSAGE_MAP()
};
