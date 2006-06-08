// LineTracer.h : main header file for the LineTracer application
//
#pragma once

#ifndef __LINETRACER_H
#define __LINETRACER_H

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CLineTracerApp:
// See LineTracer.cpp for the implementation of this class
//

class CLineTracerApp : public CWinApp
{
public:
	CLineTracerApp(void);


// Overrides
public:
	virtual BOOL InitInstance(void);

// Implementation
	afx_msg void OnAppAbout(void);
	DECLARE_MESSAGE_MAP()
};

extern CLineTracerApp theApp;

#endif
