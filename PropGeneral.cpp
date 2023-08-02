// PropGeneral.cpp : implementation file
//

#include "stdafx.h"
#include "UniPatSys.h"
#include "PropGeneral.h"
#include "UniPatSysCard.h"
#include "PropGeneral.h"
#include "CardDeckLib.h"
#include "PropGeneral.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPropGeneral property page

IMPLEMENT_DYNCREATE(CPropGeneral, CPropertyPage)

CPropGeneral::CPropGeneral() : CPropertyPage(CPropGeneral::IDD)
, m_cardback(0)
{
	//{{AFX_DATA_INIT(CPropGeneral)
	m_automove = FALSE;
	m_bestmove = -1;
	m_gametitle = _T("");
	m_game_no = 0;
	//}}AFX_DATA_INIT
}

CPropGeneral::~CPropGeneral()
{
}

void CPropGeneral::DoDataExchange(CDataExchange* pDX)
{
   CPropertyPage::DoDataExchange(pDX);

   //{{AFX_DATA_MAP(CPropGeneral)
   DDX_Control(pDX, IDC_GAME_TYPE, m_gametypes);
   DDX_Check(pDX, IDC_AUTOMOVE, m_automove);
   DDX_Radio(pDX, IDC_BEST_SINGLE, m_bestmove);
   DDX_CBString(pDX, IDC_GAME_TYPE, m_gametitle);
   DDX_Text(pDX, IDC_GAME_NO, m_game_no);
   DDX_Control(pDX, IDC_CARDBACK, m_cardbacks);
   //}}AFX_DATA_MAP
   if (!pDX->m_bSaveAndValidate) {
      CString cs;
      for (int j = 0; m_cardgame->GetTitle (j, cs); j++) {
	 m_gametypes.AddString (cs);
      }
  //    for (int j = 0; m_cardgame->GetTitle (j, cs); j++) {
	 //m_gametypes.AddString (cs);
  //    }
      vector<char*> vb = CCardDeckLib::CardDeckLib->GetBackIds();
      for (unsigned int j = 0; j < vb.size(); ++j)
	 m_cardbacks.AddString(vb[j]);
      DDX_CBString(pDX, IDC_GAME_TYPE, m_gametitle);
   }
   DDX_CBIndex(pDX, IDC_CARDBACK, m_cardback);
}


BEGIN_MESSAGE_MAP(CPropGeneral, CPropertyPage)
	//{{AFX_MSG_MAP(CPropGeneral)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
//	ON_CBN_SELCHANGE(IDC_GAME_TYPE, OnCbnSelchangeGameType)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropGeneral message handlers

