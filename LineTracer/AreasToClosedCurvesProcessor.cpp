#include "StdAfx.h"
#include "areastoclosedcurvesprocessor.h"
#include "LayerManager.h"

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
	//pseudocode deluxe!!

	//N = maximum line width
	//A = minimum area

	//copy source_bitmap to work_bitmap
	//erode work_bitmap N times
	//remove work_bitmap areas that are smaller than A
	//[1..N]
	//    dilate work_bitmap
	//	  work_bitmap &= source_bitmap
	//source_bitmap &= !work_bitmap
	//edge detect work_bitmap
	//source_bitmap |= work_bitmap

	return a_src_image;
}
