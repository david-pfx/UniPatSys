// PropGeneral.h : header file
//
/////////////////////////////////////////////////////////////////////////////
#include "afxwin.h"
#if !defined(AFX_PROPGENERAL_H__37CC8EE1_ED41_11D1_A8AA_00000100782D__INCLUDED_)
#define AFX_PROPGENERAL_H__37CC8EE1_ED41_11D1_A8AA_00000100782D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CCardGame;

/////////////////////////////////////////////////////////////////////////////
// CPropGeneral dialog

class CPropGeneral : public CPropertyPage
{
	DECLARE_DYNCREATE(CPropGeneral)

// Construction
public:
	CPropGeneral();
	~CPropGeneral();
   CCardGame* m_cardgame;

// Dialog Data
	//{{AFX_DATA(CPropGeneral)
	enum { IDD = IDD_PROP_GENERAL };
	CComboBox	m_gametypes;
	BOOL	m_automove;
	int		m_bestmove;
	CString	m_gametitle;
	int		m_game_no;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPropGeneral)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropGeneral)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
   int m_cardback;
   CComboBox m_cardbacks;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPGENERAL_H__37CC8EE1_ED41_11D1_A8AA_00000100782D__INCLUDED_)
