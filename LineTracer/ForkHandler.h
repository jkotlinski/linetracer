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
