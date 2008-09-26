#include "AreaContourizer.h"

using namespace std;

/* 
this class takes a binary image as input. its intention is to modify areas 
and thick lines, so that only the outline of these areas remain.
areas and lines smaller/thinner than given thresholds will be kept as is.

class is a singleton

johank
*/

namespace ImageProcessing {

	void RemoveSmallAreasFromImage ( CRawImage<bool> & a_image, unsigned int a_area_threshold );
	void ClearAreaAndGetClearedPixels (
		CRawImage<bool> & a_image, // in, out
		const pair<int,int> & a_start_point, // in
		deque<pair<int,int> > & a_pixels_in_area ); // out
	void ClearEverythingButOutlines ( CRawImage<bool> & a_image );

	AreaContourizer::AreaContourizer(void)
	: CImageProcessor()
	{
		SetName("Areas to Closed Curves");
		SetType(CImageProcessor::AREA_CONTOURIZER);
	}

	AreaContourizer* AreaContourizer::Instance() {
		static AreaContourizer inst;
		return &inst;
	}

	//---------------------------

	CSketchImage * AreaContourizer::Process(CProjectSettings & a_project_settings, CSketchImage * a_src_image)
	{
		static const int l_max_line_radius = 2;
		static const int l_min_area_threshold = 30;

		//copy source bitmap to work bitmap
		CRawImage<bool> * l_source_img = dynamic_cast< CRawImage<bool>* >(a_src_image);
		assert ( l_source_img );
		CRawImage<bool> * l_work_img = l_source_img->Clone();
		assert ( l_work_img );

		//remove lines thinner than l_max_line_radius from work bitmap
		for ( int l_pass = 0; l_pass < l_max_line_radius; l_pass++ ) {
			l_work_img->Erode();
		}

		RemoveSmallAreasFromImage ( *l_work_img, l_min_area_threshold );

		//restore the areas that have not been removed
		for ( int l_pass = 0; l_pass < l_max_line_radius; l_pass++ ) {
			l_work_img->Dilate();
		}

		CRawImage<bool> * l_dst_img = l_source_img->Clone();
		assert ( l_dst_img );

		//source bitmap &= !work bitmap
		for ( int l_offset=0; l_offset < l_work_img->GetPixelCount(); l_offset++ ) {
			if ( k_black == l_work_img->GetPixel(l_offset) ) {
				l_dst_img->SetPixel( l_offset, k_white );
			}
		}

		ClearEverythingButOutlines ( *l_work_img );

		//source bitmap |= work bitmap
		for ( int l_offset=0; l_offset < l_work_img->GetPixelCount(); l_offset++ ) {
			if ( k_black == l_work_img->GetPixel ( l_offset ) ) {
				l_dst_img->SetPixel( l_offset, k_black );
			}
		}

		delete l_work_img;

		return l_dst_img;
	}

	void RemoveSmallAreasFromImage(CRawImage<bool> & a_image, const unsigned int a_area_threshold) {
		CRawImage<bool> *l_tmp_image = a_image.Clone();
		assert ( l_tmp_image );

		for (int y=0; y<l_tmp_image->GetHeight(); y++) {
			for (int x=0; x<l_tmp_image->GetWidth(); x++) {
				if ( k_black == l_tmp_image->GetPixel(x,y) ) {
					deque<pair<int,int> > l_pixels_in_area;
					pair<int,int> l_start_point (x,y);

					ClearAreaAndGetClearedPixels ( 
						*l_tmp_image, l_start_point, l_pixels_in_area );

					//if cleared less than a_area_threshold pixels, clear area in
					// a_image too
					if ( l_pixels_in_area.size() < a_area_threshold ) {
						a_image.PaintPixels ( l_pixels_in_area, k_white );
					}
				}
			}
		}
		delete l_tmp_image;
	}

	void ClearAreaAndGetClearedPixels (
		CRawImage<bool> & a_image, // in, out
		const pair<int,int> & a_start_point, // in
		deque<pair<int,int> > & a_pixels_in_area // out
		)
	{
		deque<pair<int,int> > l_points_to_check;

		l_points_to_check.push_back ( a_start_point );

		while ( !l_points_to_check.empty() ) {
			const pair<int,int> p = l_points_to_check.front();
			const int x = p.first;
			const int y = p.second;
			l_points_to_check.pop_front();

			if( x<0 || y<0 ) continue;
			if( x >= a_image.GetWidth() ) continue;
			if( y >= a_image.GetHeight() ) continue;
			if( k_white == a_image.GetPixel(x,y) ) continue;

			//ok.. we're on a black pixel. store and clear it
			a_pixels_in_area.push_back( pair<int,int>(x,y) );
			a_image.SetPixel(x, y, k_white);

			//queue neighbors for examination
			l_points_to_check.push_back( pair<int,int>(x+1, y) );
			l_points_to_check.push_back( pair<int,int>(x-1, y) );
			l_points_to_check.push_back( pair<int,int>(x, y+1) );
			l_points_to_check.push_back( pair<int,int>(x, y-1) );
		}
	}

	void ClearEverythingButOutlines ( CRawImage<bool> & a_image ) {
		deque<pair<int,int> > l_pixels_to_paint_white;

		for ( int y = 1; y < a_image.GetHeight()-1; y++ ) {
			for ( int x = 1; x < a_image.GetWidth()-1; x++ ) {
				if ( k_black == a_image.GetPixel(x,y) ) {
					//do we have any white neighbor pixels? if so, don't do anything
					if ( a_image.GetPixel(x-1, y) ) continue;
					if ( a_image.GetPixel(x+1, y) ) continue;
					if ( a_image.GetPixel(x, y+1) ) continue;
					if ( a_image.GetPixel(x, y-1) ) continue;

					//no white neighbors. mark this pixel to be painted white
					l_pixels_to_paint_white.push_back( pair<int,int> ( x, y ) );
				}
			}
		}
		a_image.PaintPixels ( l_pixels_to_paint_white, k_white );
	}

}
