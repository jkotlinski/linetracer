#include "StdAfx.h"
#include ".\gaussian.h"

#include <math.h>

CGaussian* CGaussian::_instance = 0;

CGaussian::CGaussian(void)
{
	SetParam("radius",0.5);
}

CGaussian::~CGaussian(void)
{
}

CGaussian* CGaussian::Instance() {
	if(_instance == 0) {
		_instance = new CGaussian();
	}
	return _instance;
}

CRawImage* CGaussian::Process(CRawImage* src)
{
	double radius=GetParam("radius");

	CRawImage *dst=new CRawImage(src->GetWidth(), src->GetHeight());

	if(radius>0.0) {
		//smooth
		GaussianSmooth(src, dst, radius);
	} else {
		for(int i=0; i<src->GetHeight()*src->GetWidth(); i++) {
			dst->SetPixel(i,src->GetPixel(i));
		}
	}

	return dst;
}

void CGaussian::GaussianSmooth(CRawImage *src, CRawImage *dst, double sigma)
{
	int r, c, rr, cc,     /* Counter variables. */
		windowsize,        /* Dimension of the gaussian kernel. */
		center;            /* Half of the windowsize. */
	double *tempim,        /* Buffer for separable filter gaussian smoothing. */
		*kernel,        /* A one dimensional gaussian kernel. */
		dot,            /* Dot product summing variable. */
		sum;            /* Sum of the kernel weights variable. */

	//--Create a 1-dimensional gaussian smoothing kernel-----
	MakeGaussianKernel(sigma, &kernel, &windowsize);

	center = windowsize / 2;

	// Allocate a temporary buffer image and the smoothed image.

	int rows = src->GetHeight();
	int cols = src->GetWidth();

	tempim = new double[rows*cols];

	// Blur in the x - direction.
	for(r=0;r<rows;r++){
		for(c=0;c<cols;c++){
			dot = 0.0;
			sum = 0.0;
			for(cc=(-center);cc<=center;cc++){
				if(((c+cc) >= 0) && ((c+cc) < cols)){
					dot += src->GetPixel(c+cc,r) * kernel[center+cc];
					sum += kernel[center+cc];
				}
			}
			tempim[r*cols+c] = dot/sum;
		}
	}

	//Blur in the y - direction.
	for(c=0; c<cols; c++){
		for(r=0; r<rows; r++){
			sum = 0.0;
			dot = 0.0;
			for(rr=(-center);rr<=center;rr++){
				if(((r+rr) >= 0) && ((r+rr) < rows)){
					dot += tempim[(r+rr)*cols+c] * kernel[center+rr];
					sum += kernel[center+rr];
				}
			}
			dst->SetPixel(c,r,(ARGB)(dot/sum));
		}
	}

	delete tempim;
	delete kernel;
}


void CGaussian::MakeGaussianKernel(double sigma, double **kernel, int *windowsize)
{
	int i, center;
	double x, fx, sum=0.0;

	*windowsize = 1 + 2 * (int)ceil(2.5 * sigma);
	center = (*windowsize) / 2;

	*kernel = new double[*windowsize];

	for(i=0; i<(*windowsize); i++){
		x = i - center;
		fx = pow(2.71828, -0.5*x*x/(sigma*sigma)) / (sigma * sqrt(6.2831853));
		(*kernel)[i] = fx;
		sum += fx;
	}

	for(i=0; i<(*windowsize); i++) (*kernel)[i] /= sum;
}