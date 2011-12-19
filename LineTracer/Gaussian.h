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
