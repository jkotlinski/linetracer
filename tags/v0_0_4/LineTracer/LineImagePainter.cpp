#include "StdAfx.h"
#include ".\lineimagepainter.h"

#include "PolyLine.h"

#include <list>

using namespace std;

void CLineImagePainter::Paint(CRawImage<ARGB>* canvas, CLineImage* li)
{
	int color=1;

	for(int i=0; i<li->Size(); i++) {
		CPolyLine *line=li->At(i);

		int hasKnots=line->HasKnots();

		vector<CSketchPoint*>::iterator p_iter;

		CSketchPoint *p_prev = 0;

		for(p_iter=line->Begin(); p_iter!=line->End(); p_iter++) {
			CSketchPoint *p=*p_iter;

			if(p_prev) {
				int paintcolor = 0xff000000;
				if(color&4) paintcolor |= 0xff0000;
				if(color&2) paintcolor |= 0xff00;
				if(color&1) paintcolor |= 0xff;
				DrawCurve(canvas,p_prev,p,paintcolor);

				ARGB pointColor = 0xff000000;
				ARGB prevPointColor = 0xff000000;
				if(p->m_yFork) pointColor = 0xff0080ff;
				if(p_prev->m_yFork) prevPointColor = 0xff0080ff;
				DrawPoint(canvas,p,p->IsKnee()?0xff00ff00:pointColor);
				DrawPoint(canvas,p_prev,p_prev->IsKnee()?0xff00ff00:prevPointColor);
			}
			p_prev = p;
		}

		color++;
		if(color==7) color=1;
	}
}

// Extremely Fast Line Algorithm Var E (Addition Fixed Point PreCalc)
// Copyright 2001-2, By Po-Han Lin
void CLineImagePainter::DrawLine(CRawImage<ARGB>* canvas, CFPoint* start, CFPoint* end, ARGB c)
{
	int x = (int)start->x;
	int y = (int)start->y;
	int x2 = (int)end->x;
	int y2 = (int)end->y;

	if(x<0) return;
	if(y<0) return;
	if(x2<0) return;
	if(x2<0) return;
	if(x>=canvas->GetWidth()) return;
	if(x2>=canvas->GetWidth()) return;
	if(y>=canvas->GetHeight()) return;
	if(y2>=canvas->GetHeight()) return;

   	bool yLonger=false;
	int shortLen=y2-y;
	int longLen=x2-x;
	if (abs(shortLen)>abs(longLen)) {
		int swap=shortLen;
		shortLen=longLen;
		longLen=swap;				
		yLonger=true;
	}
	int decInc;
	if (longLen==0) decInc=0;
	else decInc = (shortLen << 16) / longLen;

	if (yLonger) {
		if (longLen>0) {
			longLen+=y;
			for (int j=0x8000+(x<<16);y<=longLen;++y) {
				canvas->SetPixel(j>>16,y,c);
				j+=decInc;
			}
			return;
		}
		longLen+=y;
		for (int j=0x8000+(x<<16);y>=longLen;--y) {
			canvas->SetPixel(j>>16,y,c);
			j-=decInc;
		}
		return;	
	}

	if (longLen>0) {
		longLen+=x;
		for (int j=0x8000+(y<<16);x<=longLen;++x) {
			canvas->SetPixel(x,j>>16,c);
			j+=decInc;
		}
		return;
	}
	longLen+=x;
	for (int j=0x8000+(y<<16);x>=longLen;--x) {
		canvas->SetPixel(x,j>>16,c);
		j-=decInc;
	}
}

void CLineImagePainter::DrawPoint(CRawImage<ARGB>* canvas, CFPoint* p, ARGB color)
{
	int x = (int)p->x;
	int y = (int)p->y;

	if(x == 0) return;
	if(y == 0) return;
	if(x == canvas->GetWidth()-1) return;
	if(y == canvas->GetHeight()-1) return;

	canvas->SetPixel(x,y,color);
	canvas->SetPixel(x-1,y,color);
	canvas->SetPixel(x,y-1,color);
	canvas->SetPixel(x+1,y,color);
	canvas->SetPixel(x,y+1,color);
}

void CLineImagePainter::DrawCurve(CRawImage<ARGB>* canvas, CSketchPoint* start, CSketchPoint* end, ARGB color)
{
	CFPoint curr(end->x,end->y);

	//TRACE("point 0: %i %i\n",curr.x,curr.y);

	for(double a=0.02; a<1.01; a+=0.02) {
		double b=1-a;
		
		CFPoint next(
			int(start->x*a*a*a + start->GetControlPointForward().x*3*a*a*b + end->GetControlPointBack().x*3*a*b*b + end->x*b*b*b),
			int(start->y*a*a*a + start->GetControlPointForward().y*3*a*a*b + end->GetControlPointBack().y*3*a*b*b + end->y*b*b*b)
			);

		DrawLine(canvas,&curr,&next,color);
		curr = next;
		//TRACE("point %f: %f %f\n",a,curr.x,curr.y);
	}
}
