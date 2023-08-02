// PropAdvanced.h : header file
//
/////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_PROPADVANCED_H__37CC8EE6_ED41_11D1_A8AA_00000100782D__INCLUDED_)
#define AFX_PROPADVANCED_H__37CC8EE6_ED41_11D1_A8AA_00000100782D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CPropAdvanced dialog

class CPropAdvanced : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropAdvanced)

// Construction
public:
	CPropAdvanced();
	~CPropAdvanced();

// Dialog Data
	//{{AFX_DATA(CPropAdvanced)
	enum { IDD = IDD_PROP_ADVANCED };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropAdvanced)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropAdvanced)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPADVANCED_H__37CC8EE6_ED41_11D1_A8AA_00000100782D__INCLUDED_)
