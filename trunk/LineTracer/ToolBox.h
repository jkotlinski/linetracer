#pragma once

#include "LayerManager.h"

// CToolBox

class CToolBox : public CDialogBar
{
	DECLARE_DYNAMIC(CToolBox)

private:
	CToolBox();
	virtual ~CToolBox();

protected:
	DECLARE_MESSAGE_MAP()
public:
	static CToolBox* Instance(void);
	void Init(void);
	void UpdateParams(void);
	double GetParam(int type);

	static enum ParamType { BINARIZER };
};


