#pragma once
#include "imageprocessor.h"

namespace ImageProcessing {

	class AreaContourizer :
		public CImageProcessor
	{
	protected:
		AreaContourizer(void);
	public:
		static AreaContourizer* Instance();
		CSketchImage* Process(CProjectSettings & a_project_settings, CSketchImage* src);
	};

}
