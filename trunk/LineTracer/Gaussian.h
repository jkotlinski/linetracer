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
	CSketchImage* Process(CSketchImage* src);
private:
	void MakeGaussianKernel(double sigma, double **kernel, int *windowsize);
	void GaussianSmooth(CRawImage<unsigned char> *src, CRawImage<unsigned char> *dst, double sigma);
};
