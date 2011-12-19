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

#include <deque>
#include "RawImage.h"

#define __WIN32__
#include <boost/pool/pool_alloc.hpp>

class CHoleFiller :
	public CImageProcessor
{
protected:
	CHoleFiller(void);
public:
	static CHoleFiller* Instance();
	~CHoleFiller(void);
	CSketchImage* Process(CProjectSettings & a_project_settings, CSketchImage* src);
private:
	void ScanArea(CRawImage<bool>* canvas, deque<int>* pixelsInArea, const CPoint &start, const unsigned int a_max_area) const;
};
