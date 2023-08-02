// UniPatSys.h : main header file for the UNIPATSYS application
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNIPATSYS_H__402568C5_E278_11D1_A88F_00000100782D__INCLUDED_)
#define AFX_UNIPATSYS_H__402568C5_E278_11D1_A88F_00000100782D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CUniPatSysApp:
// See UniPatSys.cpp for the implementation of this class
//

class CUniPatSysApp : public CWinApp
{
public:
	CUniPatSysApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUniPatSysApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CUniPatSysApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual int ExitInstance();
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNIPATSYS_H__402568C5_E278_11D1_A88F_00000100782D__INCLUDED_)
