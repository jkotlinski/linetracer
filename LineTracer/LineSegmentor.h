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

#include "PolyLine.h"
#include "LineImage.h"

class CLineSegmentor :
	public CImageProcessor
{
public:
	~CLineSegmentor(void);
	static CLineSegmentor* Instance(void);
protected:
	CLineSegmentor(void);
public:
	CSketchImage* Process(CProjectSettings & a_project_settings, CSketchImage* src);

private:
	void Add(CPolyLine* dst, CPolyLine* src);
};
