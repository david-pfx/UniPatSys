// UniPatSysCardView.cpp: implementation of the CCardView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UniPatSys.h"
#include "UniPatSysCard.h"
#include "UniPatSysCardView.h"
#include "UniPatSysDoc.h"
#include "UpsGameProps.h"
#include "CardDeckLib.h"
#include "UniPatSysView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CardParams - constants for card appearance
// CardLayout - constants for card layout on desktop

//char* FaceIds[] = 
//      { ""
//      ,  "#1 ", "#2",  "#3",  "#4",  "#5",  "#6",  "#7",  "#8",  "#9", "#10"
//      , "#11", "#12", "#13", "#14", "#15", "#16", "#17", "#18", "#19", "#20"
//      , "#21", "#22", "#23", "#24", "#25", "#26", "#27", "#28", "#29", "#30"
//      , "#31", "#32", "#33", "#34", "#35", "#36", "#37", "#38", "#39", "#40"
//      , "#41", "#42", "#43", "#44", "#45", "#46", "#47", "#48", "#49", "#50"
//      , "#51", "#52", NULL };
//char* BackIds [] = 	       // card backs (0 is special)
//      { "#53"     // back IDs
//      , "#54", "#55", "#56", "#57", "#58", "#59", "#60", "#61", "#62", "#63"
//      , "#64", "#65", NULL };

struct CCardParams {
   POINT base;			       // base offset from top left corner
   COLORREF bgcolour;		       // background color as 0x00bbggrr
   SIZE cardsize;		       // size of cards (must match library)
   SIZE rounding;		       // how much to round the corners
   SIZE separate;		       // spacing for separate cards
   SIZE overlap;		       // for overlapped cards to see edge
//   char* libname;		       // name of library containing card bitmaps
//   char** faceIds;	       // card fronts (0 is special)
//   char** backIds;	       // card backs (0 is special)
} cardParams = { 
  // base      bgcolour      cardsize    rounding    separate     overlap
   { 40, 40 }, 0x00007000, { 71, 96 }, {  5,  5 }, { 80, 112 }, { 16, 16 },
//   "CARDS.DLL", 
//   FaceIds, BackIds
//   "D:\\WIN31\\SYSTEM32\\CARDS.DLL",
   //{ ""	       // face IDs
   //   ,  "#1 ", "#2",  "#3",  "#4",  "#5",  "#6",  "#7",  "#8",  "#9", "#10"
   //   , "#11", "#12", "#13", "#14", "#15", "#16", "#17", "#18", "#19", "#20"
   //   , "#21", "#22", "#23", "#24", "#25", "#26", "#27", "#28", "#29", "#30"
   //   , "#31", "#32", "#33", "#34", "#35", "#36", "#37", "#38", "#39", "#40"
   //   , "#41", "#42", "#43", "#44", "#45", "#46", "#47", "#48", "#49", "#50"
   //   , "#51", "#52" },
   //{ "#53"     // back IDs
   //   , "#54", "#55", "#56", "#57", "#58", "#59", "#60", "#61", "#62", "#63"
   //   , "#64", "#65" },
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Implementation for small utility classes

class CMyBrush {
public:
   CDC* m_pDC;
   CBrush* m_brush;
   CMyBrush (CDC* pDC, CBrush* b) { m_pDC = pDC; m_brush = pDC->SelectObject (b); }
   ~CMyBrush () { m_pDC->SelectObject (m_brush); }
};

class CMyPen {
public:
   CDC* m_pDC;
   CPen* m_pen;
   CMyPen (CDC* pDC, CPen* p) { m_pDC = pDC; m_pen = pDC->SelectObject (p); }
   ~CMyPen () { m_pDC->SelectObject (m_pen); }
};

class CMyBitmap {
public:
   CDC* m_pDC;
   CBitmap* m_bitmap;
   CMyBitmap (CDC* pDC, CBitmap* p) { m_pDC = pDC; m_bitmap = pDC->SelectObject (p); }
   ~CMyBitmap () { m_pDC->SelectObject (m_bitmap); }
};

#define RGB_BLACK RGB(0,0,0)
#define RGB_WHITE RGB(255,255,255)
#define RGB_FACE RGB(255,255,255)	// face=white
#define RGB_BACK RGB(192,0,0)		// back=red
#define RGB_SPACE RGB(0,192,0)

//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CBitChunk - a bitmap chunk which knows how to cut and paste to a view
class CBitChunk {
public:
   CSize m_size;
   CBitmap* m_bitmap;
   CBitChunk () { m_size = CSize (0, 0);  m_bitmap = new CBitmap; }
   CBitChunk (CDC* pDC, CRect rect);
   ~CBitChunk () { delete m_bitmap; }
   void Cut (CDC* pDC, CRect rect);
   void Paste (CDC* pDC, CPoint point);
   void Paste (CDC* pDC, CPoint point, CRect rect);
   void Move (CDC* pDC, CBitChunk* chunku, CPoint oldpt, CPoint newpt);
   void Move (CDC* pDC, CBitChunk* chunku, CRect rect, CPoint newpt);
   void Draw (CDC* pDC, CRect rect, CView* pv);
};

CBitChunk::CBitChunk (CDC* pDC, CRect rect) { 
   m_size = rect.Size (); 
   m_bitmap = new CBitmap; 
   m_bitmap->CreateCompatibleBitmap (pDC, rect.Width (), rect.Height ());
}

void CBitChunk::Cut (CDC* pDC, CRect rect) { 
   m_size = rect.Size (); 
   m_bitmap->DeleteObject ();
   CDC memDC;
   m_bitmap->CreateCompatibleBitmap (pDC, rect.Width (), rect.Height ());
   memDC.CreateCompatibleDC (pDC);
   memDC.SelectObject (m_bitmap);
   memDC.BitBlt (0, 0, rect.Width (), rect.Height (), 
	      	 pDC, rect.left, rect.top, SRCCOPY);
}

void CBitChunk::Draw (CDC* pDC, CRect rect, CView* pv) { 
   CDC memDC;
   memDC.CreateCompatibleDC (pDC);
   memDC.SelectObject (m_bitmap);
   CRgn rgn;
   rgn.CreateRectRgnIndirect (rect);
   memDC.SelectClipRgn (&rgn);
   //memDC.FillRect (rect, m_bgbrush);
   //pv->Draw (&memDC);
}

// copy whole of chunk to point
void CBitChunk::Paste (CDC* pDC, CPoint point) { 
   CDC memDC;
   memDC.CreateCompatibleDC (pDC);
   memDC.SelectObject (m_bitmap);
   pDC->BitBlt (point.x, point.y, m_size.cx, m_size.cy, 
	        &memDC, 0, 0, SRCCOPY);
}

// repair DC by copying matching part of chunk to rect
void CBitChunk::Paste (CDC* pDC, CPoint point, CRect rect) { 
   CDC memDC;
   memDC.CreateCompatibleDC (pDC);
   memDC.SelectObject (m_bitmap);
   pDC->BitBlt (point.x, point.y, rect.Width (), rect.Height (),
	        &memDC, rect.left, rect.top, SRCCOPY);
}

void CBitChunk::Move (CDC* pDC, CBitChunk* chunku, CPoint oldpt, CPoint newpt) { 
   CDC memDC;
   memDC.CreateCompatibleDC (pDC);
   memDC.SelectObject (chunku->m_bitmap);
   pDC->BitBlt (oldpt.x, oldpt.y, m_size.cx, m_size.cy, 
	        &memDC, 0, 0, SRCCOPY);
   memDC.BitBlt (0, 0, m_size.cx, m_size.cy, 
	         pDC, newpt.x, newpt.y, SRCCOPY);
   memDC.SelectObject (m_bitmap);
   pDC->BitBlt (newpt.x, newpt.y, m_size.cx, m_size.cy, 
	      	&memDC, 0, 0, SRCCOPY);
}

void CBitChunk::Move (CDC* pDC, CBitChunk* chunku, CRect rect, CPoint newpt) { 
   CRect rctot = rect | CRect (newpt, m_size);
   CPoint offs (max (0, newpt.x - rctot.left), max (0, newpt.y - rctot.top));
   CDC memDC, memDCx;
   CBitmap bm;
   memDC.CreateCompatibleDC (pDC);
   memDC.SelectObject (chunku->m_bitmap);
   memDCx.CreateCompatibleDC (pDC);
   bm.CreateCompatibleBitmap (pDC, rctot.Width (), rctot.Height ());
   memDCx.SelectObject (&bm);
   memDCx.BitBlt (0, 0, rctot.Width (), rctot.Height (),
	          &memDC, rctot.left, rctot.top, SRCCOPY);
   memDCx.SelectObject (m_bitmap);
   memDCx.BitBlt (offs.x, offs.y, m_size.cx, m_size.cy, 
	          &memDC, 0, 0, SRCCOPY);
   pDC->BitBlt (rctot.left, rctot.top, rctot.Width (), rctot.Height (),
	        &memDCx, 0, 0, SRCCOPY);
}

/////////////////////////////////////////////////////////////////////////////
// CCardGameView - class of views of the whole card game

//IMPLEMENT_DYNCREATE(CCardGameView, CView)

CCardGameView::CCardGameView (CUniPatSysView* parent)
{
   m_view = parent;
   m_base = CPoint (cardParams.base);
   m_bgcolour = cardParams.bgcolour;
   m_dragging = false;
   m_trace = NULL;
   m_bgbrush = new CBrush (m_bgcolour);
   m_cardview = new CCardView (this);
   m_cardsize = m_cardview->CardSize ();
   m_inited = false;
   m_cardback = 1;
}

CCardGameView::~CCardGameView()
{
   Clear ();
   delete m_bgbrush;
   delete m_cardview;
}

// initialise view -- requires cardgame object
// must be called once, then call Clear()
void CCardGameView::Setup (CCardGame* cardgame) {
   m_cardgame = cardgame;
   if (m_inited)
      Clear ();
   m_client_rect = CRect (0, 0, 0, 0);
   m_client_loc = CPoint (0);
   m_layout_rect = CRect (0, 0, 0, 0);
   CPoint locs [ST_SIZE];  // next stack location
   int ords [ST_SIZE];	   // next stack ordinal
   memset (locs, 0, sizeof (locs));
   memset (ords, 0, sizeof (ords));
   for (CCardLayout* pl = m_cardgame->CardLayout (); pl->type != ST_NUL; pl++) {
      for (int ord = 1; ord <= pl->nstack; ord++) {
         CStack *ps = m_cardgame->Stack (pl->type, ords [pl->type] + ord);
	 if (!ps)
	    break;	   // no more of this type
         CStackView* psv = new CStackView (this, ps);
         m_stviews.push_back(psv);
//         m_stviews.AddTail (psv);
         psv->m_offset_pref = m_cardview->Overlap (pl->card_offset);
         psv->m_offset = psv->m_offset_pref;
         if (ord == 1)
            locs [ps->m_type] = m_base + m_cardview->Separate (pl->stack_loc);
         psv->m_loc = locs [ps->m_type];
         locs [ps->m_type] += m_cardview->Separate (pl->next_stack);
         psv->m_maxsize = m_cardview->Separate (pl->max_size);
	 m_layout_rect |= CRect (psv->m_loc, psv->m_maxsize);
	 psv->m_visible = true;
      }
      ords [pl->type] += pl->nstack;
   }
   m_layout_rect.OffsetRect (m_base);
   m_layout_rect.InflateRect (CSize (m_base));
   m_inited = true;
}

void CCardGameView::Clear () {
   while (!m_stviews.empty()) {
      delete m_stviews.back();
      m_stviews.pop_back();
   }
   //while (!m_stviews.IsEmpty ()) 
   //   delete m_stviews.RemoveHead ();
   m_inited = false;
}

/////////////////////////////////////////////////////////////////////////////
// CCardGameView diagnostics

#ifdef _DEBUG
void CCardGameView::AssertValid() const {
   ASSERT (m_cardview);
   ASSERT (m_bgbrush);
   ASSERT (m_cardview->m_parent == this);
   m_bgbrush->AssertValid ();
   m_cardview->AssertValid ();
   for (vector<CStackView*>::const_iterator it = m_stviews.begin(); 
        it != m_stviews.end(); ++it) {
      ASSERT ((*it)->m_parent == this);
      (*it)->AssertValid ();
   }
   //for (int i = 0; i < m_stviews.size(); ++i) {
   //   CStackView* psv = m_stviews.GetNext (vpos);
   //   ASSERT (m_stviews[0]->m_parent == this);
   //   m_stviews[0]->AssertValid ();
   //}
   //POSITION vpos = m_stviews.GetHeadPosition ();
   //while (vpos) {
   //   CStackView* psv = m_stviews.GetNext (vpos);
   //   ASSERT (psv->m_parent == this);
   //   psv->AssertValid ();
   //}
}

void CStackView::AssertValid() const {
}

void CCardView::AssertValid() const {
}

void CCardGameView::Dump(CDumpContext& dc) const {
   dc << " background=" << m_bgbrush
      << " cardsize=" << m_cardsize
      << " dragging=" << m_dragging
      << " client rect=" << m_client_rect
      << " loc=" << m_client_loc << "\n"
      << " dragstack=" << m_dragstack
      << " fromsv=" << m_drag_from << "\n"
      << " cardview=" << m_cardview;
}

void CStackView::Dump(CDumpContext& dc) const {
   dc << " rect=" << m_rect
      << " top=" << m_rect_top
      << " loc=" << m_loc
      << " offset pref=" << m_offset_pref
      << " actual=" << m_offset;
}

void CCardView::Dump(CDumpContext& dc) const {
   //dc << " libname=" << m_libname
   //   << " card size=" << m_cardsize
   //   << " hinst=" << int (m_hinst);
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CCardGameView - update, draw, stack view stuff

bool CCardGameView::GetToolTipText (char* buf, int maxl) {
    return true;
}

bool CCardGameView::GetToolTipText (CPoint pt, CRect& rc, CString& cs) {
   CStackView* psv;
   CCard* pcard;
   int ord;
   if (FindStackView (pt, psv)) {
      if (psv->FindCard (pt, pcard, rc, ord) &&
	 pcard->TestFlags (CF_FACEUP)) {
	    cs.Format ("%s of %ss", pcard->RankText (), pcard->SuitText ());
         } else {
	    cs.Format ("%s", psv->m_stack->Text ());
            rc = psv->m_rect;
         }
      return true;
   }
   return false;
}

void CCardGameView::Update (long lHint, CObject* pHint) {
   switch (lHint & ~hintArgMask) {
   case HT_NUL:
      break;
   case HT_INIT:
      Setup (m_view->CardGame ());
      m_view->InvalidateRect (NULL);
      break;
   case HT_STACK:
      if (m_inited) {
      CStack* ps = (CStack*) pHint;
	 if (ps == NULL || ps->m_type == ST_NUL)
	    break;
	 CStackView* psv = GetStackView (ps);
	 if (psv)
	    psv->Update (lHint & hintArgMask);
      }
      break;
   default: ;
      ASSERT (false);
   }
}

bool CCardGameView::SetCursor () {
   ::SetCursor (LoadCursor (NULL, IDC_ARROW));
   return true;
}

// given a stack, find the stack which views it (or NULL)
CStackView* CCardGameView::GetStackView (CStack* ps) {
   for (vector<CStackView*>::const_iterator it = m_stviews.begin(); 
        it != m_stviews.end(); ++it) {
      if (ps == (*it)->m_stack)
         return *it;
   }
   //POSITION vpos = m_stviews.GetHeadPosition ();
   //while (vpos) {
   //   CStackView* psv = m_stviews.GetNext (vpos);
   //   if (ps == psv->m_stack)
   //      return psv;
   //}
   return NULL;
}

// given a stack, find and return the stackview which views it
bool CCardGameView::FindStackView (CStack* ps, CStackView*& psview) {
   for (vector<CStackView*>::const_iterator it = m_stviews.begin(); 
        it != m_stviews.end(); ++it) {
      if (ps == (*it)->m_stack)
         psview = *it;
         return true;
   }
   //POSITION vpos = m_stviews.GetHeadPosition ();
   //while (vpos) {
   //   CStackView* psv = m_stviews.GetNext (vpos);
   //   if (ps == psv->m_stack) {
   //      psview = psv;
   //      return true;
   //   }
   //}
   return false;
}

// given a point, find and return the stack it is in
bool CCardGameView::FindStackView (CPoint point, CStackView*& psview) {
   for (vector<CStackView*>::const_iterator it = m_stviews.begin(); 
        it != m_stviews.end(); ++it) {
      if ((*it)->m_rect.PtInRect (point)) {
         psview = *it;
         return true;
      }
   }
   //POSITION vpos = m_stviews.GetHeadPosition ();
   //while (vpos) {
   //   CStackView* psv = m_stviews.GetNext (vpos);
   //   if (psv->m_rect.PtInRect (point)) {
   //      psview = psv;
   //      return true;
   //   }
   //}
   return false;
}

// given a rect, find and return the first stack it is found in
bool CCardGameView::FindStackView (CRect rect, CStackView*& psview) {
   for (vector<CStackView*>::const_iterator it = m_stviews.begin(); 
        it != m_stviews.end(); ++it) {
      if (!((*it)->m_rect & rect).IsRectEmpty ()) {
         psview = (*it);
         return true;
      }
   }
   //POSITION vpos = m_stviews.GetHeadPosition ();
   //while (vpos) {
   //   CStackView* psv = m_stviews.GetNext (vpos);
   //   if (!(psv->m_rect & rect).IsRectEmpty ()) {
   //      psview = psv;
   //      return true;
   //   }
   //}
   return false;
}

// draw all the stack views on the DC
void CCardGameView::Draw (CDC* pDC) {
   for (vector<CStackView*>::const_iterator it = m_stviews.begin(); 
        it != m_stviews.end(); ++it) {
      if ((*it)->m_visible) {
	 (*it)->UpdateSize ();
	 if (pDC->RectVisible ((*it)->m_rect))
	    (*it)->Draw (pDC);
      }
   }
  // POSITION vpos = m_stviews.GetHeadPosition ();
  // while (vpos) {
  //    CStackView* psv = m_stviews.GetNext (vpos);
  //    if (psv->m_visible) {
	 //psv->UpdateSize ();
	 //if (pDC->RectVisible (psv->m_rect))
	 //   psv->Draw (pDC);
  //    }
  // }
}

// draw a pattern on the DC inside the rect
void CCardGameView::DrawZigZag (CDC* pDC, CRect rc) {
   int aZigzag [] = { 0xFF, 0xF7, 0xEB, 0xDD, 0xBE, 0x7F, 0xFF, 0xFF };
   CBitmap bmp;
   bmp.CreateBitmap (8, 8, 1, 1, aZigzag);
   CBrush br;
   br.CreatePatternBrush (&bmp);
   br.UnrealizeObject ();
   CBrush *brold = pDC->SelectObject (&br);
   pDC->PatBlt (rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, PATCOPY);
   pDC->SelectObject (brold);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CCardGameView - clicking and dragging

void CCardGameView::MakeBestMove (CPoint point) {
   CStackView* psv;
   if (m_dragging) 
      DragExit ();
   if (FindStackView (point, psv)) {
      int ord;
      if (!psv->FindCard (point, ord))
	 ord = 0;
      m_cardgame->BestMove (psv->m_stack, ord);
   }
}

void CCardGameView::DragBegin (CPoint point) {
   CStackView* psv;
   int ord;
   if (m_dragging) 
      DragExit ();
   if (FindStackView (point, psv) && psv->FindCard (point, ord)) {
      CStack *ps = psv->m_stack;
      if (ord == 1 && ps->HeadCard ()->TestFlags (CF_FACEDOWN)) {
         if (m_cardgame->ValidTurnup (ps))
//         if (m_cardgame->m_rules->ValidTurnup (ps))
	    m_cardgame->DoSetFlags(ps, CF_FACEUP);
//	    ps->SetFlags (CF_FACEUP);
      } else if (m_cardgame->ValidSplit (ps, ord))
         DragInit (psv, ord, point);
   }
}

void CCardGameView::DragInit (CStackView* psv, int ord, CPoint point) {
   m_drag_from = psv;
   m_dragnum = ord;
   m_dragging = true;
   ::SetCursor (LoadCursor (GetModuleHandle (NULL), MAKEINTRESOURCE (IDC_DRAG_CARDS)));
   m_view->SetCapture ();
   CClientDC cDC (m_view);
   psv->Select (&cDC, ord);
//   m_dragstack = m_cardgame->Stack (ST_DRAG, 0);
//   ASSERT_VALID (m_dragstack);
//   m_dragstack->CopyStack (psv->m_stack, ord);
}

void CCardGameView::DragMove (CPoint point) {
   CStackView* psv;
   if (FindStackView (point, psv) && 
       psv != m_drag_from &&
       m_cardgame->ValidMove(psv->m_stack, m_drag_from->m_stack, m_dragnum))
       //m_cardgame->ValidJoin (psv->m_stack, m_dragstack))
      ::SetCursor (LoadCursor (GetModuleHandle (NULL), MAKEINTRESOURCE (IDC_DROP_CARDS)));
   else
      ::SetCursor (LoadCursor (GetModuleHandle (NULL), MAKEINTRESOURCE (IDC_DRAG_CARDS)));
   _ASSERTE (m_dragging);
}

void CCardGameView::DragDrop (CPoint point) {
   _ASSERTE (m_dragging);
   CStackView* psv;
   //int num = m_dragstack->GetCount ();
   if (FindStackView (point, psv) && 
       psv != m_drag_from &&
       m_cardgame->ValidMove(psv->m_stack, m_drag_from->m_stack, m_dragnum)) {
       //m_cardgame->ValidJoin (psv->m_stack, m_dragstack)) {
      m_cardgame->DoMoveStack (psv->m_stack, m_drag_from->m_stack, m_dragnum);
      //m_cardgame->DoMoveStack (psv->m_stack, m_drag_from->m_stack, num);
      //psv->m_stack->MoveStack (m_drag_from->m_stack, num);
   } else
      m_drag_from->m_stack->NotifyChange (m_dragnum);
      //m_drag_from->m_stack->NotifyChange (num);
//   m_dragstack->RemoveAll (); 
//   m_dragstack = NULL; 
   ReleaseCapture ();
   m_dragging = false;
}

void CCardGameView::DragExit () {
   if (m_dragging)
      DragDrop (OFFSCREEN);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Implementation for CStackView

// trigger redraw of (part of) stack -- hint is how many cards changed
void CStackView::Update (long lHint) {
   CRect r = m_rect_top;	// start here in case stack shrinks
   if (UpdateSize () || lHint == 0) {
      r |= m_rect;
   } else r |= CalcSize ((int)lHint);
   m_parent->m_view->InvalidateRect (r, true);
}   

void CStackView::Draw (CDC* pDC) {
   CPoint loc = m_loc;
   CCard *pchead = m_stack->HeadCard ();
   if (!pchead) 
      m_parent->m_cardview->DrawCardSpace (pDC, loc);
   else {
      POSITION cpos = m_stack->m_cardlist.GetTailPosition ();
      while (cpos) {
      	 CCard* pc = m_stack->m_cardlist.GetPrev (cpos);
	 if (pc->TestFlags (CF_VISIBLE) || pc == pchead) {
            if (pc->TestFlags (CF_FACEUP))
               m_parent->m_cardview->DrawCardFace (pDC, loc, pc->Ord ());
	    else {
	       m_parent->m_cardview->DrawCardBack (pDC, loc, 
		  CGameProps::GameProps._cardback);
	       // draw id on back, for debugging
      	       CString s;
               s.Format ("%c%d", pc->SuitChar(), pc->Rank());
//               s.Format ("%c%d", "?CDHS"[pc->Suit ()], pc->Rank ());
	       pDC->SetBkColor (RGB_WHITE);
	       pDC->SetTextColor (RGB_BLACK);
               pDC->DrawText (s, -1, CRect (loc + CSize (0, 1), 
				     CSize (m_parent->m_cardsize)), 
			      DT_TOP|DT_CENTER);
	    }
	    if (pc->TestFlags (CF_VISIBLE))
	       loc += m_offset;
	 }
      }
   } 
}

// draw pattern to indicate selection of top N cards
void CStackView::Select (CDC* pDC, int num) {
   _ASSERTE (num <= m_stack->GetCount ());
   CRect rc = CalcSize (num);
   CPen p (PS_DOT, 1, RGB_BLACK);
   CMyPen mp (pDC, &p);
   CBrush b;
   b.CreateStockObject (NULL_BRUSH);
   CMyBrush br (pDC, &b);
   pDC->RoundRect (rc, m_parent->m_cardview->m_rounding);
#ifdef PATBLT_WAY
   CBitmap bm;
   CBrush b (RGB (64, 64, 64));
   CMyBrush mb (pDC, &b);
   pDC->PatBlt (rc.left, rc.top, rc.Width (), rc.Height (), PATINVERT);
#endif
}

CStackView::CStackView (CCardGameView* pv, CStack* ps) {
   m_parent = pv; 
   m_stack = ps; 
   m_frozen = false;
}

// recalculate size of whole stack, return true if changed
bool CStackView::UpdateSize () {
   CRect r (m_loc, m_parent->m_cardsize);
   CRect rect = r;
   CRect rtop = r;
   CSize offs = (m_frozen) ? m_offset : m_offset_pref;
   int ncvis = 0;
   if (m_visible && !m_stack->IsEmpty ()) {
      for (;;) {
         rtop = rect = r = CRect (m_loc, m_parent->m_cardsize);
         POSITION cpos = m_stack->m_cardlist.GetTailPosition ();
         while (cpos) {
      	    CCard* pc = m_stack->m_cardlist.GetPrev (cpos);
	    rtop = r;
            rect |= r;
	    if (pc->TestFlags (CF_VISIBLE)) {
	       ncvis++;
	       r += CPoint (offs);
	    }
	 }
	 if (m_maxsize == CSize (0) || offs == CSize (0))
	    break;
	 if (rect.Size ().cx <= m_maxsize.cx && rect.Size ().cy <= m_maxsize.cy)
	    break;
	 ASSERT (!m_frozen);
	 offs.cx = max (0, offs.cx - 1);
	 offs.cy = max (0, offs.cy - 1);
      }
   }
   m_rect_top = rtop;
   m_rect = rect;
   if (m_offset == offs)
      return false;
   m_offset = offs;
   return true;
}

// calculate size of part stack (already Updated)
CRect CStackView::CalcSize (int ord) {
   CRect rect = m_rect_top;
   CRect r = m_rect_top;
   if (!m_stack->IsEmpty ()) {
      CCard* pchead = m_stack->m_cardlist.GetHead ();
      rect = r = m_rect_top;
      POSITION cpos = m_stack->m_cardlist.GetHeadPosition ();
      while (cpos && ord-- > 0) {
         CCard* pc = m_stack->m_cardlist.GetNext (cpos);
         if (pc->TestFlags (CF_VISIBLE) && pc != pchead) 
	    r -= CPoint (m_offset);
         rect |= r;
      }
   }
   return rect;
}

// given a point in a view of a stack, find the card, rect and ordinal
bool CStackView::FindCard (CPoint point, CCard*& pcard, CRect& rect, int& ord) {
   CCard* pchead = m_stack->HeadCard ();
   if (!pchead)
      return false;
   CRect r, rx, rr;
   int ct = 0;
   POSITION cpos = m_stack->m_cardlist.GetHeadPosition ();
   while (cpos) {
      CCard* pc = m_stack->m_cardlist.GetNext (cpos);
      ct++;
      rx = r;
      if (pc == pchead) {
         r = m_rect_top;
	 rr = r;
      } else if (pc->TestFlags (CF_VISIBLE) && pc != pchead) {
         r -= CPoint (m_offset);
         rr.SubtractRect (r, rx);
      }
      if (rr.PtInRect (point)) {
         pcard = pc;
         rect = rr;
	 ord = ct;
         return true;
      }
   }
   return false;
}

bool CStackView::FindCard (CPoint point, CCard*& pcard, CRect& rect) {
   int ord;
   return FindCard (point, pcard, rect, ord);
}

bool CStackView::FindCard (CPoint point, int& ord) {
   CCard* pcard;
   CRect rect;
   return FindCard (point, pcard, rect, ord);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Implementation for CCardView

CCardView::CCardView (CCardGameView *pv) {
   m_parent = pv;
   m_mask = NULL;
//   m_cardsize = CSize (cardParams.cardsize);
   m_rounding = CPoint (cardParams.rounding);
   m_separate = CSize (cardParams.separate);	
   m_overlap = CSize (cardParams.overlap);
//   m_libname = cardParams.libname;
//   m_faceids = cardParams.faceIds;
//   m_backids = cardParams.backIds;
   memset (m_facebmps, 0, sizeof (m_facebmps));
   memset (m_backbmps, 0, sizeof (m_backbmps));
   CCardDeckLib::Create();
   m_cardsize = CCardDeckLib::CardDeckLib->GetCardSize();
//   m_hinst = LoadLibrary (m_libname);
//   TRACE ("Hinst=%x err=%d", m_hinst, GetLastError ());
}

CCardView::~CCardView () {
   int i;
   if (m_mask)
      delete m_mask;
   for (i = 0; i <= MAXFACES; i++)
      if (m_facebmps [i])
         delete m_facebmps [i];
   for (i = 0; i <= MAXBACKS; i++)
      if (m_backbmps [i])
         delete m_backbmps [i];
}

// create a mask suitable for later blitting
// it's a black rounded rect, white filled, on a black background
void CCardView::create_mask (CDC* pDC, CBitmap*& bmp) {
   bmp = new CBitmap ();
   bmp->CreateCompatibleBitmap (pDC, m_cardsize.cx, m_cardsize.cy);
   CDC memDC;
   memDC.CreateCompatibleDC (pDC);
   memDC.SelectObject (bmp);
   memDC.PatBlt (0, 0, m_cardsize.cx, m_cardsize.cy, WHITENESS);
   //CMyPen p (&memDC, &CPen (PS_INSIDEFRAME, 1, RGB_BLACK));
   memDC.SelectObject (bmp);
   CBrush hb;
   hb.CreateStockObject (BLACK_BRUSH);
   CMyBrush b (&memDC, &hb);
   memDC.RoundRect (CRect (CPoint (0), m_cardsize), m_rounding);
}

// load bitmap into memory
// note: copy the bitmap to a compatible to force any conversions now
void CCardView::load_bmp (CDC* pDC, CString id, CBitmap*& bmp) {
   if (!m_mask)
      create_mask (pDC, m_mask);
   bmp = new CBitmap ();
   bmp->CreateCompatibleBitmap (pDC, m_cardsize.cx, m_cardsize.cy);
   CDC memDC;
   memDC.CreateCompatibleDC (pDC);
   memDC.SelectObject (bmp);
   { // pick up the bitmap from the loaded resource
   CBitmap bmpt;
   bmpt.Attach (LoadBitmap (CCardDeckLib::CardDeckLib->GetHInst(), id));
   CDC memDCt;
   memDCt.CreateCompatibleDC (pDC);
   memDCt.SelectObject (&bmpt);
   memDC.BitBlt (0, 0, m_cardsize.cx, m_cardsize.cy,
       	         &memDCt, 0, 0, SRCCOPY);
   // erase everything outside mask, i.e. white part of source
   memDCt.SelectObject (m_mask);
   memDC.BitBlt (0, 0, m_cardsize.cx, m_cardsize.cy,
       	         &memDCt, 0, 0, 0x220326);	// DSna
   }
   { // draw a frame around it, just inside masked area
   CPen p (PS_INSIDEFRAME, 1, RGB_BLACK);
   CMyPen pp (&memDC, &p);
   CBrush hb;
   hb.CreateStockObject (NULL_BRUSH);
   CMyBrush b (&memDC, &hb);
   memDC.RoundRect (CRect (CPoint (0), m_cardsize), m_rounding);
   }
}

// draw a card at a location using a bitmap, loaded if necessary
void CCardView::draw_bmp (CDC* pDC, CPoint point, CString id, CBitmap*& bmp) {
   if (bmp == NULL)
      load_bmp (pDC, id, bmp);
   CDC memDC;
   memDC.CreateCompatibleDC (pDC);
   // first use the mask to punch a hole: black=black white=keep dest
   memDC.SelectObject (m_mask);
   pDC->BitBlt (point.x, point.y, m_cardsize.cx, m_cardsize.cy,
       	        &memDC, 0, 0, SRCAND);
   // then paint the card with i-or: black edges leave dest alone
   memDC.SelectObject (bmp);		// ior src to dest
   pDC->BitBlt (point.x, point.y, m_cardsize.cx, m_cardsize.cy,
       	        &memDC, 0, 0, SRCPAINT);
}

// draw a card at a location using rectangle, brush and pen
void CCardView::draw_rect (CDC* pDC, CPoint point, COLORREF rgb, int ord) {
   CRect r (point, m_cardsize);
   if (ord == 0) {
      CPen p (PS_DOT, 1, RGB_BLACK);
      CMyPen mp (pDC, &p);
      CBitmap bm;
      unsigned int bits[] = { 0xffffffff, 0xffefffff, 0xffffffff, 0xfffeffff };
      bm.CreateBitmap (8, 8, 1, 1, bits);
      CBrush b (&bm);
      //CBrush b (HS_DIAGCROSS, RGB_BLACK);
      CMyBrush mb (pDC, &b);
      pDC->SetBkColor (m_parent->m_bgcolour);
      pDC->RoundRect (r, m_rounding);
   } else {
      CCard c (ord);
      CMyBrush b (pDC, &CBrush (rgb));
      pDC->RoundRect (r, m_rounding);
      if (rgb == RGB_FACE) {
         CString s;
	 pDC->SetBkColor (RGB_WHITE);
         s.Format ("%c%d", c.SuitChar(), c.Rank());
//         s.Format ("%c%d", "?CDHS"[c.Suit ()], c.Rank ());
         pDC->DrawText (s, -1, r, DT_TOP|DT_CENTER);
      }
   }
}

void CCardView::DrawCardFace (CDC* pDC, CPoint point, int ord) {
   if (CCardDeckLib::CardDeckLib->GetHInst()) 
      draw_bmp (pDC, point, 
	 CCardDeckLib::CardDeckLib->GetFaceIds()[ord], m_facebmps [ord]);
      //draw_bmp (pDC, point, m_faceids [ord], m_facebmps [ord]);
   else
      draw_rect (pDC, point, RGB_FACE, ord);
}

void CCardView::DrawCardBack (CDC* pDC, CPoint point, int ord) {
   if (CCardDeckLib::CardDeckLib->GetHInst()) 
      draw_bmp (pDC, point, 
	 CCardDeckLib::CardDeckLib->GetBackIds()[ord], m_backbmps [ord]);
      //draw_bmp (pDC, point, m_backids [ord], m_backbmps [ord]);
   else
      draw_rect (pDC, point, RGB_BACK, ord);
}

void CCardView::DrawCardSpace (CDC* pDC, CPoint point) {
   draw_rect (pDC, point, RGB_SPACE, 0);
}

// end of UniPatSysCardView.cpp
