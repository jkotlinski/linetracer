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
	static CGaussian* _instance;
public:
	CRawImage* Process(CRawImage* src);
private:
	void MakeGaussianKernel(double sigma, double **kernel, int *windowsize);
	void GaussianSmooth(CRawImage *src, CRawImage *dst, double sigma);
};
