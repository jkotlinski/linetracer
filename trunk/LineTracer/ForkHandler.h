#pragma once
#include "imageprocessor.h"

#include "FPoint.h"
#include "LineImage.h"

#include <vector>

class CForkHandler :
	public CImageProcessor
{
protected:
	CForkHandler(void);
	~CForkHandler(void);
public:
	static CForkHandler* Instance();
	CSketchImage* Process(CSketchImage *src);
private:
	vector<CFPoint>* Find3Forks(CLineImage *li);
public:
	vector<CFPoint> *ThreeForks;
	vector<CFPoint>* TForks;
	vector<CFPoint>* YForks;
private:
	CLineImage* HandleTForks(CLineImage* li);
public:
	CFPoint SmoothLine(CPolyLine* line, int index);
private:
	void SetEndPoint(CPolyLine* pl, CFPoint p);
	CLineImage* HandleYForks(CLineImage* li);
	void MarkYFork(CPolyLine* line, CFPoint p, int median);
	void HandleYFork(CLineImage* img, CPolyLine* baseLine, CPolyLine* line1, CPolyLine* line2, CFPoint p);
};
