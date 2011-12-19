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
