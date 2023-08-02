// UniPatSysView.h : interface of the CUniPatSysView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNIPATSYSVIEW_H__402568CD_E278_11D1_A88F_00000100782D__INCLUDED_)
#define AFX_UNIPATSYSVIEW_H__402568CD_E278_11D1_A88F_00000100782D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CUniPatSysDoc;
class CCardGameView;

class CUniPatSysView : public CView
{
// Attributes
public:
   CCardGameView* m_cardgameview;
   CBrush* m_background;
   CPoint m_client_loc;
   CRect m_client_rect;

protected: // create from serialization only
	CUniPatSysView();
	DECLARE_DYNCREATE(CUniPatSysView)

// Operations
public:
   CUniPatSysDoc* GetDocument();
   CCardGame* CardGame () { return GetDocument ()->m_cardgame; }
   int ShowOptions (int page = 0);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUniPatSysView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUniPatSysView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CUniPatSysView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEditDeal();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnNotifyNeedText(NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg void OnUpdateEditDeal(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatusGameno(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatusDealno(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatusRoundno(CCmdUI* pCmdUI);
	afx_msg void OnEditOptions();
	afx_msg void OnEditRestart();
	afx_msg void OnUpdateEditRestart(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnEditNextGame();
	afx_msg void OnEditChooseGame();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in UniPatSysView.cpp
inline CUniPatSysDoc* CUniPatSysView::GetDocument()
   { return (CUniPatSysDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNIPATSYSVIEW_H__402568CD_E278_11D1_A88F_00000100782D__INCLUDED_)
