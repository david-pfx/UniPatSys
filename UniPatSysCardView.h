// UniPatSysCardView.h: interface for the CCardView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNIPATSYSCARDVIEW_H__402568D6_E278_11D1_A88F_00000100782D__INCLUDED_)
#define AFX_UNIPATSYSCARDVIEW_H__402568D6_E278_11D1_A88F_00000100782D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

const int MAXFACES = 52;
const int MAXBACKS = 12;
#define OFFSCREEN CPoint(10000,10000)

class CStackView;
class CBitChunk;
class CCardView;
class CCardGameView;
class CUniPatSysView;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CCardView: class of how to view a card

class CCardView : public CObject
{
// Attributes
public:
   CCardGameView* m_parent;
//   CString m_libname;		 // library to use for bitmaps in this view
   CSize m_cardsize;		 // size of a card in this view
   CPoint m_rounding;		 // defines rounding of corners
   CSize m_separate;		 // offset for separated cards
   CSize m_overlap;		 // offset for overlapped cards
//   HINSTANCE m_hinst;
//   char** m_faceids;
//   char** m_backids;
   CBitmap* m_facebmps [MAXFACES+1];
   CBitmap* m_backbmps [MAXBACKS+1];
   CBitmap* m_mask;

// constructors
public:
   CCardView (CCardGameView *pv);
   ~CCardView ();
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

// Operations
public:
   CSize CardSize () { return m_cardsize; }
   CSize Separate (CSize s) { return CSize (s.cx*m_separate.cx/10, s.cy*m_separate.cy/10); }
   CSize Overlap (CSize s) { return CSize (s.cx <= 2 ? s.cx : s.cx*m_overlap.cx/10, 
					   s.cy <= 2 ? s.cy : s.cy*m_overlap.cy/10); }
   void DrawCardFace (CDC *pDC, CPoint point, int ord);
   void DrawCardBack (CDC *pDC, CPoint point, int ord);
   void DrawCardSpace (CDC *pDC, CPoint point);

   // implementation
private:
   void create_mask (CDC* pDC, CBitmap*& bmp);
   void load_bmp (CDC* pDC, CString id, CBitmap*& bmp);
   void draw_bmp (CDC* pDC, CPoint point, CString id, CBitmap*& bmp);
   void draw_rect (CDC* pDC, CPoint point, COLORREF rgb, int ord);

};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CStackView: class of how to view a stack of cards

class CStackView : public CObject {
// Attributes
public:
   CCardGameView* m_parent;
   CStack* m_stack;
   CRect m_rect;		 // total size of displayed stack
   CRect m_rect_top;		 // size of top card
   CPoint m_loc;		 // location of first card (bottom)
   CSize m_offset_pref;		 // preferred card offset (if card edge visible)
   CSize m_offset;		 // actual card offset (to fit in maxsize)
   CSize m_maxsize;		 // maximum stack size (compact if larger)
   bool m_frozen;		 // if TRUE do not recalculate offset 
   bool m_visible;		 // if not TRUE, invisible stack, do not try to display

// constructors
public:
   CStackView ();
   CStackView (CCardGameView* pv, CStack* ps);
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

// Operations
public:
   void Draw (CDC* pDC);
   void Select (CDC*, int num);
   bool UpdateSize ();
   CRect CalcSize (int ord);
   bool FindCard (CPoint point, CCard*& pcard, CRect& rect, int& ord);
   bool FindCard (CPoint point, CCard*& pcard, CRect& rect);
   bool FindCard (CPoint point, int& ord);
   void Update (long lHint);

// implementation
private:

};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CCardGameView
class CCardGameView : public CObject {
// Attributes
public:
   CUniPatSysView* m_view;	 // parent view
   vector<CStackView*> m_stviews;
//   CTypedPtrList<CObList, CStackView*> m_stviews;
   CCardView* m_cardview;	 // child
   CCardGame* m_cardgame;	 // partner
   bool m_inited;
   CBrush* m_bgbrush;
   COLORREF m_bgcolour;
   CSize m_cardsize;		 // dimensions of a card
   CPoint m_base;		// base location for layout
   CRect m_layout_rect;		// initial/default size for layout
   CRect m_client_rect;		// client area when last drawn
   CPoint m_client_loc;		// location of client area in screen coords
   CBitChunk* m_bitc;		// chunk to paste when dragging
   CBitChunk* m_bitcu;		// chunk cut from drag rect
   bool m_dragging;
   CStack* m_dragstack;		// stack of cards being dragged
   CStackView* m_drag_from;	// where stack of cards came from
   int m_dragnum;               // number of cards being dragged
   CWnd* m_trace;
   CString m_classname;
   int m_cardback;		// ordinal of id to use as card back

// constructors
public:
   CCardGameView(CUniPatSysView* parent);
   virtual ~CCardGameView();

// Operations
public:
   //CCardGame* GetDocument();
   void Setup (CCardGame* pDoc);
   void Clear ();
   CStackView* NewStackView (CStack* ps);
   CStackView* GetStackView (CStack* ps);
   bool FindStackView (CStack* ps, CStackView*& psview);
   bool FindStackView (CPoint point, CStackView*& psview);
   bool FindStackView (CRect rect, CStackView*& psview);
   bool GetToolTipText (CPoint pt, CRect& rc, CString& cs);
   bool GetToolTipText (char* buf, int maxl);
   void Draw (CDC* pDC);
   void DrawZigZag (CDC* pDC, CRect rect);
   void Update (long lHint, CObject* pHint);
   void DragBegin (CPoint point);
   bool Dragging () { return m_dragging; };
   void DragMove (CPoint point);
   void DragDrop (CPoint point);
   void DragExit ();
   void MakeBestMove (CPoint point);
   bool SetCursor ();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Implementation
private:
   void DragInit (CStackView* psv, int ord, CPoint point);

};

#endif // !defined(AFX_UNIPATSYSCARDVIEW_H__402568D6_E278_11D1_A88F_00000100782D__INCLUDED_)

// end of UniPatSysCardView.h