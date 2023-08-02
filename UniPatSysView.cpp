// UniPatSysView.cpp : implementation of the CUniPatSysView class
//

#include "stdafx.h"
#include "UniPatSys.h"
#include "UniPatSysCard.h"
#include "UniPatSysDoc.h"
#include "UniPatSysCardView.h"
#include "UpsGameProps.h"
#include "PropGeneral.h"
#include "PropAdvanced.h"
#include "UniPatSysView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUniPatSysView

IMPLEMENT_DYNCREATE(CUniPatSysView, CView)

BEGIN_MESSAGE_MAP(CUniPatSysView, CView)
	//{{AFX_MSG_MAP(CUniPatSysView)
	ON_WM_CREATE()
	ON_COMMAND(ID_EDIT_DEAL, OnEditDeal)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_SETCURSOR()
        ON_NOTIFY(TTN_NEEDTEXT, 0, OnNotifyNeedText)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DEAL, OnUpdateEditDeal)
	ON_COMMAND(ID_EDIT_OPTIONS, OnEditOptions)
	ON_COMMAND(ID_EDIT_RESTART, OnEditRestart)
	ON_UPDATE_COMMAND_UI(ID_EDIT_RESTART, OnUpdateEditRestart)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_NEXT_GAME, OnEditNextGame)
	ON_COMMAND(ID_EDIT_CHOOSE_GAME, OnEditChooseGame)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_GAMENO, OnUpdateStatusGameno)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_DEALNO, OnUpdateStatusDealno)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ROUNDNO, OnUpdateStatusRoundno)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUniPatSysView construction/destruction

CUniPatSysView::CUniPatSysView()
{
   m_cardgameview = new CCardGameView (this);
}

CUniPatSysView::~CUniPatSysView()
{
   delete m_cardgameview;
}

BOOL CUniPatSysView::PreCreateWindow(CREATESTRUCT& cs)
{
   // main aim here is to switch off the REDRAW bits so we get smooth update
   // also disable the cursor -- we set that dynamically
   LPCTSTR pcls = AfxRegisterWndClass (CS_DBLCLKS, 0, 
      ::CreateSolidBrush (m_cardgameview->m_bgcolour)); 
   cs.lpszClass = pcls;
   return CView::PreCreateWindow(cs);
}

int CUniPatSysView::OnCreate (LPCREATESTRUCT lpCreateStruct) {
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
   MoveWindow (lpCreateStruct->x, lpCreateStruct->y, 200, 500);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CUniPatSysView drawing

void CUniPatSysView::OnDraw(CDC* pDC)
{
	CUniPatSysDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
   m_cardgameview->Draw (pDC);
}

// Called after the view is first attached to the document, but it is initially displayed
void CUniPatSysView::OnInitialUpdate() 
{
   CView::OnInitialUpdate();
   m_cardgameview->Setup (CardGame());
   EnableToolTips (TRUE);
}

void CUniPatSysView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
   m_cardgameview->Update (lHint, pHint);
}

// Called to detemine whether a point is in the bounding rectangle of the specified tool
int CUniPatSysView::OnToolHitTest(CPoint point, TOOLINFO* pTI) const {
   static CRect prevrc (0, 0, 0, 0);
   static int uniqueid = 0;
   //static uniqueid = 0; --vs8: C4430
   // observations as of VS7.1
   // TOOLINFO passed in with no useful info
   // each different "tool" or region of the window must have unique, stable ID 
   // -- either: handle and TTF_IDISHWND; or any integer and rect
   // ->change as of 7x: ADDTOOL then DELTOOL, so unchanged it deletes what was just added
   // return value must be stable for this tool
   // malloc and return a string, since MFC will free it

   pTI->hwnd = m_hWnd;
   pTI->uFlags = 0;
   //pTI->uFlags = TTF_ALWAYSTIP|TTF_NOTBUTTON;
   pTI->uId = 0;
   pTI->lpszText = NULL;

   CString cs;
   CRect rc;
   if (m_cardgameview->GetToolTipText (point, rc, cs)) {
      if (rc != prevrc || prevrc.IsRectEmpty())
         ++uniqueid;
      pTI->uId = uniqueid;
      pTI->rect = prevrc = rc;
      pTI->lpszText = (char *)malloc (80);
      strcpy (pTI->lpszText, cs);
      //Beep(1000, 20);
      return pTI->uId;
   } 
   prevrc.SetRectEmpty();
   return -1;

#if 0
   if (rc.IsRectEmpty()) {
      SetRect (&rc, point.x - 10, point.y - 10, point.x + 10, point.y + 10);
      CClientDC cDC ((CWnd*)this);
      cDC.Rectangle (&rc);
      pTI->rect = rc;
      return 1;
   } else {
      pTI->rect = rc;
      if (rc.PtInRect (point))
	 return 1;
      rc.SetRectEmpty();
      return -1;
   }
#endif
}

// Sent by a ToolTip control to retrieve information needed to display a ToolTip window. 
// Identical to TTN_GETDISPINFO. Sent in the form of a WM_NOTIFY message. 

void CUniPatSysView::OnNotifyNeedText(NMHDR* pNotifyStruct, LRESULT* result) { 
   LPTOOLTIPTEXT lpt = (LPTOOLTIPTEXT)pNotifyStruct;
   m_cardgameview->GetToolTipText (lpt->szText, 80);
   Beep(1600, 20);
   *result = 0;
}

// called when window has moved or resized
// WINDOWPOS is incomprehensible!
// re-validate previously drawn area unless absolute screen location changed
#ifdef LATER
void CUniPatSysView::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
   GetClientRect (m_client_rect);
   m_client_loc = m_client_rect.TopLeft();
   ClientToScreen (&m_client_loc);
   CView::OnWindowPosChanging (lpwndpos);
}

void CUniPatSysView::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
   CPoint pt (0);		// top left corner
   ClientToScreen (&pt);
   if (m_client_loc == pt)
      ValidateRect (m_client_rect);
   CView::OnWindowPosChanged(lpwndpos);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CUniPatSysView printing

BOOL CUniPatSysView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CUniPatSysView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CUniPatSysView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CUniPatSysView diagnostics

#ifdef _DEBUG
void CUniPatSysView::AssertValid() const
{
	CView::AssertValid();
}

void CUniPatSysView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CUniPatSysDoc* CUniPatSysView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CUniPatSysDoc)));
	return (CUniPatSysDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CUniPatSysView message handlers

// right button 
void CUniPatSysView::OnRButtonDown(UINT nFlags, CPoint point) 
{
}

// left button 
void CUniPatSysView::OnLButtonDown(UINT nFlags, CPoint point) {
   m_cardgameview->m_cardgame->SetUndoBegin();
   if (nFlags == MK_LBUTTON)
      m_cardgameview->DragBegin (point);
   else if (nFlags & MK_CONTROL) {
      m_cardgameview->DragExit();
      m_cardgameview->MakeBestMove (point);
   }
   m_cardgameview->m_cardgame->SetUndoEnd();
}

void CUniPatSysView::OnMouseMove(UINT nFlags, CPoint point) {
   if (m_cardgameview->Dragging())
      m_cardgameview->DragMove (point);
}

void CUniPatSysView::OnLButtonUp(UINT nFlags, CPoint point) {
   m_cardgameview->m_cardgame->SetUndoBegin();
   if (m_cardgameview->Dragging())
      m_cardgameview->DragDrop (point);
   if (CGameProps::GameProps._bestmove_sclick)
      m_cardgameview->MakeBestMove (point);
   m_cardgameview->m_cardgame->CheckFinished();
   m_cardgameview->m_cardgame->SetUndoEnd();
}

void CUniPatSysView::OnRButtonUp(UINT nFlags, CPoint point) 
{
}

void CUniPatSysView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
   m_cardgameview->DragExit();
   m_cardgameview->m_cardgame->SetUndoBegin();
   if (nFlags == MK_LBUTTON && !CGameProps::GameProps._bestmove_sclick)
   //if (nFlags == MK_LBUTTON && !m_cardgameview->m_cardgame->m_bestmove_sclick)
      m_cardgameview->MakeBestMove (point);
   m_cardgameview->m_cardgame->CheckFinished();
   m_cardgameview->m_cardgame->SetUndoEnd();
}

void CUniPatSysView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
   switch (nChar) {
   case VK_ESCAPE:
      AfxGetMainWnd()->ShowWindow(SW_MINIMIZE);
      break;
   //case VK_SPACE:
   //   CardGame()->DealCards (CardGame()->Stack (ST_TAB, 1), 1);	 
   //   break;
   //case VK_F7:
   //   SetWindowPos (NULL, 0, 0, 100, 100, 0);
   //   CFrameWnd *pfr = (CFrameWnd*)AfxGetThread()->m_pMainWnd;
   //   pfr->RecalcLayout();
   //   break;
   }
   CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

// set cursor whenever needed
BOOL CUniPatSysView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
   return m_cardgameview->SetCursor();
}

void CUniPatSysView::OnEditDeal() {
   m_cardgameview->m_cardgame->SetUndoBegin();
   CardGame()->DoDeal();
   m_cardgameview->m_cardgame->SetUndoEnd();
}

void CUniPatSysView::OnUpdateEditDeal(CCmdUI* pCmdUI) 
{
   pCmdUI->Enable (CardGame()->ValidDeal());
}

void CUniPatSysView::OnUpdateStatusGameno (CCmdUI* pCmdUI) {
   CUniPatSysDoc* pdoc = (CUniPatSysDoc*)GetDocument();
   if (pdoc) {
      CString cs;
      cs.Format ("Game %d", CardGame()->m_gameno);
      pCmdUI->SetText (cs);
      pCmdUI->Enable (CardGame()->m_gameno > 0);
   }
}

void CUniPatSysView::OnUpdateStatusDealno (CCmdUI* pCmdUI) {
   CUniPatSysDoc* pdoc = (CUniPatSysDoc*)GetDocument();
   if (pdoc) {
      CString cs;
      cs.Format ("Deal %d", CardGame()->DealNo());
      pCmdUI->SetText (cs);
      pCmdUI->Enable (CardGame()->DealNo());
   }
}

void CUniPatSysView::OnUpdateStatusRoundno (CCmdUI* pCmdUI) {
   CUniPatSysDoc* pdoc = (CUniPatSysDoc*)GetDocument();
   if (pdoc) {
      CString cs;
      cs.Format ("Round %d", CardGame()->RoundNo());
      pCmdUI->SetText (cs);
      pCmdUI->Enable (CardGame()->RoundNo());
   }
}

int CUniPatSysView::ShowOptions (int page) {
   CUniPatSysDoc* pdoc = (CUniPatSysDoc*)GetDocument();
   CPropertySheet pps ("UniPatSys Options");
   CPropGeneral ppg1;
   pps.AddPage (&ppg1);
   CPropAdvanced ppg2;
   pps.AddPage (&ppg2);
   pps.SetActivePage (page);
   ppg1.m_cardgame = CardGame();
   ppg1.m_automove = CGameProps::GameProps._automove;
   ppg1.m_bestmove = (CGameProps::GameProps._bestmove_sclick) ? 2 : 1;
   ppg1.m_gametitle = CGameProps::GameProps._game_name;
   ppg1.m_game_no = CGameProps::GameProps._seed;
   ppg1.m_cardback = CGameProps::GameProps._cardback;
   CString cs;
   if (pps.DoModal() == IDOK) {
      CGameProps::GameProps._automove = !!ppg1.m_automove;
      CGameProps::GameProps._bestmove_sclick = (ppg1.m_bestmove == 2);
      CGameProps::GameProps._game_name = ppg1.m_gametitle;
      CGameProps::GameProps._seed = ppg1.m_game_no;
      CGameProps::GameProps._cardback = ppg1.m_cardback;
      CardGame()->NewGame(CGameProps::GameProps._game_name, 
			  CGameProps::GameProps._seed);
      return IDOK;
   }
   return IDCANCEL;
}

void CUniPatSysView::OnEditOptions() {
   ShowOptions();
//   }
}

void CUniPatSysView::OnEditRestart() {
   CUniPatSysDoc* pdoc = (CUniPatSysDoc*)GetDocument();
   CardGame()->RestartGame ();
}

void CUniPatSysView::OnUpdateEditRestart (CCmdUI* pCmdUI) {
   CUniPatSysDoc* pdoc = (CUniPatSysDoc*)GetDocument();
   pCmdUI->Enable (CardGame()->m_dealno > 0);
}

void CUniPatSysView::OnEditUndo() {
   CardGame()->Undo();
}

void CUniPatSysView::OnUpdateEditUndo (CCmdUI* pCmdUI) {
   pCmdUI->Enable (CardGame()->CanUndo());
}

void CUniPatSysView::OnEditRedo() {
   CardGame()->Redo();
}

void CUniPatSysView::OnUpdateEditRedo (CCmdUI* pCmdUI) {
   pCmdUI->Enable (CardGame()->CanRedo());
}

void CUniPatSysView::OnEditNextGame() 
{
   CUniPatSysDoc* pdoc = (CUniPatSysDoc*)GetDocument();
   CardGame()->NextGame();
}

void CUniPatSysView::OnEditChooseGame() 
{
   ShowOptions (0);
}

// end of UniPatSysView.cpp
