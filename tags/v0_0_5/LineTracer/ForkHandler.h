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
public:
	static CForkHandler* Instance();
	CSketchImage* Process(CSketchImage *src);
private:
	vector<CFPoint>* Find3Forks(const CLineImage *li) const;
	vector<CFPoint>* ThreeForks;
	vector<CFPoint>* TForks;
	vector<CFPoint>* YForks;
	CLineImage* HandleTForks(const CLineImage* li);
	void SetEndPoint(CPolyLine* pl, const CFPoint &p) const;
	CLineImage* HandleYForks(const CLineImage* li);
	void MarkYFork(CPolyLine* line, const CFPoint &p, int median) const;
	void HandleYFork(CLineImage* img, CPolyLine* baseLine, CPolyLine* line1, CPolyLine* line2, const CFPoint &p);
public:
	void PaintImage(CSketchImage* a_image, CRawImage<ARGB> *a_canvas) const;
};