/** This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

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
	CSliderCtrl c_binarizerThresholdControl;
	CSliderCtrl c_detailEnhanceControl;
	//CSliderCtrl c_holeFillerControl;
	CSliderCtrl c_tailPrunerControl;
	CSliderCtrl c_curveDetailControl;
	
	CButton c_moveButton;
	CButton c_zoomButton;

	CButton c_viewOriginalLayerButton;
	CButton c_viewAllLayersButton;
	CButton c_viewVectorLayerButton;
public:
	double GetParam(CProjectSettings::ParamName a_name) const;

protected:
	virtual void DoDataExchange(CDataExchange *pDX);
	virtual BOOL OnInitDialogBar();
	afx_msg LRESULT OnUpdateToolboxDataFromLayers ( WPARAM wParam, LPARAM lParam );
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMReleasedCaptureDetailEnhanceSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnToolboxChangeBwthreshold(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDrawHoleFillerSlider(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLineLengthSlider(NMHDR *pNMHDR, LRESULT *pResult);
	void SetLineTracerView(CLineTracerView* a_view);
private:
	CLineTracerView* m_lineTracerView;
	bool m_isInitialized;
public:
	afx_msg void OnCurveDetailSlider(NMHDR *pNMHDR, LRESULT *pResult);
	void MoveButtonClicked();
	void ZoomButtonClicked();
	void ViewOriginalLayerButtonClicked();
	void ViewVectorLayerButtonClicked();
private:
	bool m_viewOriginalLayerButtonIsDown;
	bool m_viewVectorLayerButtonIsDown;
public:
	void ViewAllLayersButtonClicked(void);
private:
	void ResetLayerViewButtons(void);
public:
	bool IsVectorLayerVisible(void);
	bool IsOriginalLayerVisible(void);
};
