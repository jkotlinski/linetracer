#pragma once

#include "LayerManager.h"
#include "InitDialogBar.h"
#include "LineTracerView.h"

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
	CSpinButtonCtrl c_binarizerThresholdControl;
	CSliderCtrl c_detailEnchanceControl;
	CSliderCtrl c_holeFillerControl;
	CSliderCtrl c_tailPrunerControl;
	CSliderCtrl c_curveDetailControl;

public:
	double GetParam(CProjectSettings::ParamName a_name) const;

protected:
	virtual void DoDataExchange(CDataExchange *pDX);
	virtual BOOL OnInitDialogBar();
	afx_msg LRESULT OnUpdateToolboxDataFromLayers ( WPARAM wParam, LPARAM lParam );
	afx_msg void OnNMReleasedCaptureDetailEnchanceSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnToolboxChangeBwthreshold();
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDrawHoleFillerSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLineLengthSlider(NMHDR *pNMHDR, LRESULT *pResult);
	void SetLineTracerView(CLineTracerView* a_view);
private:
	CLineTracerView* m_lineTracerView;
	bool m_isInitialized;
public:
	afx_msg void OnCurveDetailSlider(NMHDR *pNMHDR, LRESULT *pResult);
};
