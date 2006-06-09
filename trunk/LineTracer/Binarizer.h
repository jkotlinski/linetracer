#pragma once
#include "imageprocessor.h"

namespace ImageProcessing {

class CBinarizer :
	public CImageProcessor
{
protected:
	CBinarizer(void);
public:
	static CBinarizer* Instance();
	~CBinarizer(void);
	CSketchImage* Process(CProjectSettings & a_project_settings, CSketchImage* src);

private:
	int CalculateOtsuThreshold(const CRawImage<unsigned char> *img) const;
	int* m_sketchBoard;
public:
	void Init(void);
private:
	bool m_isInitialized;
public:
	void Reset(void);
private:
	int CalculateKittlerThreshold(const CRawImage<unsigned char> * a_img);
};

}
