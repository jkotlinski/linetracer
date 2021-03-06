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

#include "StdAfx.h"
#include "gaussian.h"

#include <math.h>
#include "ProjectSettings.h"
#include "LayerManager.h"

CGaussian::CGaussian(void)
: CImageProcessor()
{
	//LOG("init gaussian\n");
	SetName( CString ( "Gaussian" ) );
}

CGaussian::~CGaussian(void)
{
}

CGaussian* CGaussian::Instance() {
    static CGaussian inst;
    return &inst;
}

CSketchImage* CGaussian::Process(CProjectSettings & a_project_settings, CSketchImage* i_src)
{
	CRawImage<unsigned char> *src=dynamic_cast<CRawImage<unsigned char>*>(i_src);
	ASSERT ( src != NULL );

	double radius = a_project_settings.GetParam(
		CProjectSettings::GAUSSIAN_RADIUS);

	CRawImage<unsigned char> *dst=new CRawImage<unsigned char>(src->GetWidth(), src->GetHeight());

	if(radius>0.0) {
		//smooth
		GaussianSmooth(src, dst, static_cast<double>(radius));
	} else {
		for(int i=0; i<src->GetHeight()*src->GetWidth(); i++) {
			dst->SetPixel(i,src->GetPixel(i));
		}
	}

	return dst;
}

void CGaussian::GaussianSmooth(CRawImage<unsigned char> *src, CRawImage<unsigned char> *dst, double sigma)
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
			dst->SetPixel(c,r,static_cast<unsigned char>(dot/sum));
		}
	}

	delete[] tempim;
	delete kernel;
}


void CGaussian::MakeGaussianKernel(double sigma, double **kernel, int *windowsize)
const
{
	int i, center;
	double x, fx, sum=0.0;

	*windowsize = 1 + 2 * static_cast<int> ( ceil ( 2.5 * sigma ) );
	center = (*windowsize) / 2;

	*kernel = new double[*windowsize];

	for(i=0; i<(*windowsize); i++){
		x = double(i - center);
		fx = double(pow(2.71828, -0.5*x*x/(sigma*sigma)) / (sigma * sqrt(6.2831853)));
		(*kernel)[i] = fx;
		sum += fx;
	}

	for(i=0; i<(*windowsize); i++) (*kernel)[i] /= sum;
}
