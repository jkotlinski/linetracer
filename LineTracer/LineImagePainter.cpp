#include "StdAfx.h"
#include "lineimagepainter.h"

#include "PolyLine.h"

using namespace std;

void CLineImagePainter::Paint(CRawImage<ARGB>* canvas, const CLineImage* li)
{
	int color=1;

	LOG("li->Size(): %i\n" , li->Size());
	for(unsigned int i=0; i<li->Size(); i++) 
	{
		CPolyLine *line=li->At(i);

		vector<CSketchPoint*>::iterator p_iter;

		CSketchPoint *p_prev = 0;

		for(p_iter=line->Begin(); p_iter!=line->End(); ++p_iter) 
		{
			CSketchPoint *p=*p_iter;

			if(p_prev) {
				ARGB paintcolor = 0xff000000;
				if(color&4) paintcolor |= 0xff0000;
				if(color&2) paintcolor |= 0xff00;
				if(color&1) paintcolor |= 0xff;
				DrawCurve(canvas,p_prev,p,paintcolor);

				/*
				ARGB pointColor = 0xff000000;
				ARGB prevPointColor = 0xff000000;
				if(p->m_isYFork) pointColor = 0xff0080ff;
				if(p_prev->m_isYFork) prevPointColor = 0xff0080ff;
				DrawPoint(canvas,p,p->IsKnee()?0xff00ff00:pointColor);
				DrawPoint(canvas,p_prev,p_prev->IsKnee()?0xff00ff00:prevPointColor);
				*/
			}
			p_prev = p;
		}

		color++;
		if(color==7) color=1;
	}
}

// Extremely Fast Line Algorithm Var E (Addition Fixed Point PreCalc)
// Copyright 2001-2, By Po-Han Lin
void CLineImagePainter::DrawLine(CRawImage<ARGB>* canvas, const CPoint &start, const CPoint &end, ARGB c)
{
	int x = start.x;
	int y = start.y;
	const int x2 = end.x;
	const int y2 = end.y;

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
		int l_swap=shortLen;
		shortLen=longLen;
		longLen=l_swap;				
		yLonger=true;
	}
	int decInc;
	if (longLen==0) decInc=0;
	else decInc = (shortLen << 16) / longLen;

	if (yLonger) {
		if (longLen>0) {
			longLen+=y;
			for (unsigned int j=0x8000+(x<<16);y<=longLen;++y) {
				canvas->SetPixel(j>>16,y,c);
				j+=decInc;
			}
			return;
		}
		longLen+=y;
		for (unsigned int j=0x8000+(x<<16);y>=longLen;--y) {
			canvas->SetPixel(j>>16,y,c);
			j-=decInc;
		}
		return;	
	}

	if (longLen>0) {
		longLen+=x;
		for (unsigned int j=0x8000+(y<<16);x<=longLen;++x) {
			canvas->SetPixel(x,j>>16,c);
			j+=decInc;
		}
		return;
	}
	longLen+=x;
	for (unsigned int j=0x8000+(y<<16);x>=longLen;--x) {
		canvas->SetPixel(x,j>>16,c);
		j-=decInc;
	}
}

void CLineImagePainter::DrawPoint(CRawImage<ARGB>* canvas, const CFPoint* p, ARGB color)
{
	static const bool drawFatPoint = false;

	int x = (int)p->GetX();
	int y = (int)p->GetY();

	if(x == 0) return;
	if(y == 0) return;
	if(x == canvas->GetWidth()-1) return;
	if(y == canvas->GetHeight()-1) return;

	canvas->SetPixel(x,y,color);

	//lint -e{506} const ok!
	if ( ( drawFatPoint ) )
	{
		canvas->SetPixel(x-1,y,color);
		canvas->SetPixel(x,y-1,color);
		canvas->SetPixel(x+1,y,color);
		canvas->SetPixel(x,y+1,color);
	}
}

void CLineImagePainter::DrawCurve(CRawImage<ARGB>* canvas, const CSketchPoint* start, const CSketchPoint* end, ARGB color)
{
	CPoint curr( int ( end->GetX() ), int ( end->GetY() ) );

	//LOG("point 0: %i %i\n",curr.x,curr.y);

	for(double a=0.0; a<=1.0; a+=0.05) {
		const double b=1.0-a;
		const double l_ab = a*b;
		const double l_aaa = a*a*a;
		const double l_bbb = b*b*b;

		const CFPoint l_secondPoint = start->GetControlPointForward();
		const CFPoint l_thirdPoint = end->GetControlPointBack();

		CPoint next(
			int(start->GetX()*l_aaa + l_secondPoint.GetX()*3.0*l_ab*a + l_thirdPoint.GetX()*3.0*b*l_ab + end->GetX()*l_bbb),
			int(start->GetY()*l_aaa + l_secondPoint.GetY()*3.0*l_ab*a + l_thirdPoint.GetY()*3.0*b*l_ab + end->GetY()*l_bbb)
			);

		DrawLine(canvas,curr,next,color);
		curr = next;
		//LOG("point %f: %f %f\n",a,curr.x,curr.y);
	}
}
