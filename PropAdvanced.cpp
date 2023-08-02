// PropAdvanced.cpp : implementation file
//

#include "stdafx.h"
#include "UniPatSys.h"
#include "PropAdvanced.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropAdvanced property page

IMPLEMENT_DYNCREATE(CPropAdvanced, CPropertyPage)

CPropAdvanced::CPropAdvanced() : CPropertyPage(CPropAdvanced::IDD)
{
	//{{AFX_DATA_INIT(CPropAdvanced)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CPropAdvanced::~CPropAdvanced()
{
}

void CPropAdvanced::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropAdvanced)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropAdvanced, CPropertyPage)
	//{{AFX_MSG_MAP(CPropAdvanced)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropAdvanced message handlers
