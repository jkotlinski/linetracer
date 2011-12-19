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

#include <map>
#include "ProjectSettings.h"
#include "RawImage.h"
using namespace std;
class CImageProcessor
{
public:
	CImageProcessor(void);
	virtual ~CImageProcessor(void);

	virtual CSketchImage* Process(CProjectSettings & a_project_settings, CSketchImage* src) = 0;

public:

	enum Type { 
		DESATURATOR, 
		//GAUSSIAN, 
		BINARIZER, 
		HOLEFILLER,
		AREA_CONTOURIZER,
		THINNER, 
		SKELETONIZER, 
		TAILPRUNER, 
		LINESEGMENTOR, 
		FORKHANDLER, 
		KNEESPLITTER, 
		BEZIERMAKER,
		INVALID_TYPE
	};

private:
	CString m_name;
	bool m_IsValid;
public:
	const CString* GetName(void);
	void SetName(const CString &a_name);
	CImageProcessor::Type GetType(void);
protected:
	void SetType( CImageProcessor::Type a_type );
private:
	CImageProcessor::Type m_type;
};
