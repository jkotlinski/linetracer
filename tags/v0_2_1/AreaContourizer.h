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
		~AreaContourizer(void);
		CSketchImage* Process(CSketchImage* src);
	};

}
