#pragma once
#include "imageprocessor.h"

class CGaussian :
	public CImageProcessor
{
public:
	~CGaussian(void);
	static CGaussian* Instance(void);
protected:
	CGaussian(void);
public:
	CSketchImage* Process(CProjectSettings & a_project_settings, CSketchImage* src);
private:
	void MakeGaussianKernel(double sigma, double **kernel, int *windowsize) const;
	void GaussianSmooth(CRawImage<unsigned char> *src, CRawImage<unsigned char> *dst, double sigma);
};
