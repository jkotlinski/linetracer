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

#include "FPoint.h"
#include "LineImage.h"
#include "SketchImage.h"
#include <vector>

class CForkHandler :
	public CImageProcessor
{
protected:
	CForkHandler(void);
	~CForkHandler(void);

private: //NO COPY OR ASSIGNMENT!
	CForkHandler&  operator = (const CForkHandler& other);
	//lint -e{1704} private
	CForkHandler(const CForkHandler& other);
public:
	static CForkHandler* Instance();
	CSketchImage* Process(CProjectSettings & a_project_settings, CSketchImage *src);
private:
	vector<CFPoint>* Find3Forks(const CLineImage *li) const;
	vector<CFPoint>* ThreeForks;
	vector<CFPoint>* YForks;
	CLineImage* HandleTForks(const CLineImage* li);
	CLineImage* HandleYForks(const CLineImage* li);
	void MarkPointsWithLineWidthGreaterThanMedian(CPolyLine* line, const CFPoint &p) const;
	void HandleFoundYFork(CLineImage* img, CPolyLine* baseLine, CPolyLine* line1, CPolyLine* line2, const CFPoint &p);
};
