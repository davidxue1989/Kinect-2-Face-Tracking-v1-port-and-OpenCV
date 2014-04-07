
// FT2OpenCV.h : main header file for the FT2OpenCV application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CFT2OpenCVApp:
// See FT2OpenCV.cpp for the implementation of this class
//

class CFT2OpenCVApp : public CWinApp
{
public:
	CFT2OpenCVApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CFT2OpenCVApp theApp;
