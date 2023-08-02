// UniPatSysDoc.cpp : implementation of the CUniPatSysDoc class
//

#include "stdafx.h"
#include "UniPatSys.h"
#include "UniPatSysCard.h"
#include "UniPatSysDoc.h"
#include "UpsGameProps.h"
#include "UniPatSysDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUniPatSysDoc

IMPLEMENT_DYNCREATE(CUniPatSysDoc, CDocument)

BEGIN_MESSAGE_MAP(CUniPatSysDoc, CDocument)
	//{{AFX_MSG_MAP(CUniPatSysDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUniPatSysDoc construction/destruction

CUniPatSysDoc::CUniPatSysDoc() {
   m_serialised = false;
   m_cardgame = new CCardGame (this);
}

CUniPatSysDoc::~CUniPatSysDoc() {
   delete m_cardgame;
}

// Called as part of the File New command
BOOL CUniPatSysDoc::OnNewDocument() {
   if (!CDocument::OnNewDocument())
      return FALSE;
   // set up default game (for now)
   m_cardgame->NewGame(CGameProps::GameProps._game_name, 
		       CGameProps::GameProps._seed);
   m_cardgame->NextGame();
   CGameProps::Save();
   return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CUniPatSysDoc serialization

void CUniPatSysDoc::Serialize(CArchive& ar) {
   CObject::Serialize(ar);
   m_cardgame->Serialize(ar);
   m_serialised = true;
}

/////////////////////////////////////////////////////////////////////////////
// CUniPatSysDoc diagnostics

#ifdef _DEBUG
void CUniPatSysDoc::AssertValid() const {
   CDocument::AssertValid();
   m_cardgame->AssertValid ();
}

void CUniPatSysDoc::Dump(CDumpContext& dc) const {
   CDocument::Dump(dc);
   m_cardgame->Dump (dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUniPatSysDoc commands

// Called as part of the File Open command
BOOL CUniPatSysDoc::OnOpenDocument(LPCTSTR lpszPathName) {
   if (!CDocument::OnOpenDocument(lpszPathName))
      return FALSE;
   // seralisation does all the work
   return TRUE;
}

// Called to delete the document's data without destroying the object itself
void CUniPatSysDoc::DeleteContents() {
   m_cardgame->Clear();
   m_serialised = false;
   CDocument::DeleteContents();
}

// Called before a modified document is to be closed
BOOL CUniPatSysDoc::SaveModified() {
   // don't ask about saving unless it was saved or loaded
   if (m_serialised) 
      return CDocument::SaveModified();
   return TRUE;
}

// end of UniPatSysDoc.cpp

