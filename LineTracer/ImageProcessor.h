#pragma once

#include <map>
#include "RawImage.h"
using namespace std;
class CImageProcessor
{
public:
	CImageProcessor(void);
	virtual ~CImageProcessor(void);

	virtual CSketchImage* Process(CSketchImage* src) = 0;

public:

	enum Type { 
		DESATURATOR, 
		//GAUSSIAN, 
		BINARIZER, 
		HOLEFILLER,
		AREA_CONTOURIZER,
		THINNER, 
		SKELETONIZER, 
		TAILPRUNER, 
		LINESEGMENTOR, 
		FORKHANDLER, 
		KNEESPLITTER, 
		BEZIERMAKER,
		INVALID_TYPE
	};

private:
	CString m_name;
	bool m_IsValid;
public:
	const CString* GetName(void);
	void SetName(const CString &a_name);
	CImageProcessor::Type GetType(void);
protected:
	void SetType( CImageProcessor::Type a_type );
private:
	CImageProcessor::Type m_type;
};
