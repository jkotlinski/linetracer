#include "StdAfx.h"
#include "areastoclosedcurvesprocessor.h"
#include "LayerManager.h"

#include <deque>

using namespace std;


AreasToClosedCurvesProcessor::AreasToClosedCurvesProcessor(void)
: CImageProcessor()
{
	SetName(CString("Areas to Closed Curves"));
	SetType(CImageProcessor::AREAS_TO_CLOSED_CURVES);
}

AreasToClosedCurvesProcessor::~AreasToClosedCurvesProcessor(void)
{
}

AreasToClosedCurvesProcessor* AreasToClosedCurvesProcessor::Instance() {
    static AreasToClosedCurvesProcessor inst;
    return &inst;
}

//---------------------------

CSketchImage* AreasToClosedCurvesProcessor::Process(CSketchImage *a_src_image)
{
	static const int l_maximumLineRadius = 2;
	static const int l_minimumAreaThreshold = 30;

	//copy source_bitmap to work_bitmap
	CRawImage<bool> *l_sourceImage = static_cast< CRawImage<bool>* >(a_src_image);
	ASSERT ( l_sourceImage != NULL );
	CRawImage<bool> *l_workImage = l_sourceImage->Clone();
	CRawImage<bool> *l_dstImage = l_sourceImage->Clone();

	//erode work_bitmap N times
	for ( int l_erosionPass = 0; 
		l_erosionPass < l_maximumLineRadius;
		l_erosionPass++ )
	{
		l_workImage->Erode();
	}

	//remove work_bitmap areas that are smaller than A
	ASSERT ( l_workImage != NULL );
	RemoveSmallAreasFromImage ( *l_workImage, l_minimumAreaThreshold );

	//[1..N]
	for ( int l_dilationPass = 0; 
		l_dilationPass < l_maximumLineRadius;
		l_dilationPass++ )
	{
	//    dilate work_bitmap
		l_workImage->Dilate();
	}

	//source_bitmap &= !work_bitmap
	for ( int l_offset=0; l_offset<l_workImage->GetPixelCount(); l_offset++)
	{
		if ( l_workImage->GetPixel(l_offset) == false )
		{
			//workimage pixel == black --> paint target white
			l_dstImage->SetPixel(l_offset, true);
		}
	}

	//edge detect work_bitmap
	// == remove all black pixels with only black neighbors
	deque<pair<int,int> > l_pixelsToPaintWhite;

	for ( int y = 1; y < l_workImage->GetHeight()-1; y++ )
	{
		for ( int x = 1; x < l_workImage->GetWidth()-1; x++ )
		{
			if ( false == l_workImage->GetPixel(x,y) )
			{
				//pixel is black

				//check for white neighbor pixels
				if ( l_workImage->GetPixel(x-1, y) ) continue;
				if ( l_workImage->GetPixel(x+1, y) ) continue;
				if ( l_workImage->GetPixel(x, y+1) ) continue;
				if ( l_workImage->GetPixel(x, y-1) ) continue;

				//mark pixel to be painted white
				l_pixelsToPaintWhite.push_back( pair<int,int> ( x, y ) );
			}
		}
	}
	while ( !l_pixelsToPaintWhite.empty() )
	{
		pair<int,int> l_pixelPos = l_pixelsToPaintWhite.front();
		l_pixelsToPaintWhite.pop_front();
		//paint white
		l_workImage->SetPixel(l_pixelPos.first, l_pixelPos.second, true);
	}

	//source_bitmap |= work_bitmap
	for ( int l_offset=0; l_offset<l_workImage->GetPixelCount(); l_offset++)
	{
		if ( l_workImage->GetPixel(l_offset) == false )
		{
			//black
			l_dstImage->SetPixel(l_offset, false);
		}
	}

	delete l_workImage;

	return l_dstImage;
}

void AreasToClosedCurvesProcessor::RemoveSmallAreasFromImage(CRawImage<bool> & a_image, 
															 const unsigned int a_minimumAreaThreshold)
{
	//CRawImage<bool> *dst = new CRawImage<bool>(src->GetWidth(),src->GetHeight());
	CRawImage<bool> *tmp = a_image.Clone();

	for(int x=0; x<tmp->GetWidth(); x++) {
		for(int y=0; y<tmp->GetHeight(); y++) {
			if(!tmp->GetPixel(x,y)) {
				deque<int> pixelsInArea;
				
				ScanArea(*tmp, &pixelsInArea, CPoint(x,y), a_minimumAreaThreshold);

				if(pixelsInArea.size() < a_minimumAreaThreshold) 
				{
					while(!pixelsInArea.empty()) 
					{
						unsigned int l_p = pixelsInArea.front();
						int l_px = l_p & 0xffff;
						int l_py = l_p >> 16;
						a_image.SetPixel(l_px, l_py, true);
						pixelsInArea.pop_front();
					}
				}
			}
		}
	}
	delete tmp;
}

void AreasToClosedCurvesProcessor::ScanArea(CRawImage<bool> &canvas, deque<int>* pixelsInArea, const CPoint &start, const unsigned int a_max_area)
const
{
	deque<int> pointsToCheck;

	pointsToCheck.push_back(start.x | (start.y<<16));
	pixelsInArea->push_back(start.x | (start.y<<16));

	while(!pointsToCheck.empty()) 
	{
		int p = pointsToCheck.front();
		int x = p&0xffff;
		int y = p>>16;
		pointsToCheck.pop_front();

		if( x<0 || y<0 ) continue;
		if( x == canvas.GetWidth() ) continue;
		if( y == canvas.GetHeight() ) continue;
		if( canvas.GetPixel(x,y) ) continue;

		if(pixelsInArea->size() <= a_max_area) 
		{
			pixelsInArea->push_back(x | (y<<16));
		}
		canvas.SetPixel(x, y, true);

		pointsToCheck.push_back( (x+1) | (y<<16));
		pointsToCheck.push_back( (x-1) | (y << 16));
		pointsToCheck.push_back( x | ((y+1)<<16 ) );
		pointsToCheck.push_back( x | ((y-1)<<16 ) );
	}
}
