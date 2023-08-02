// UniPatSysCard.h: interface for the CCard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UNIPATSYSCARD_H__402568D5_E278_11D1_A88F_00000100782D__INCLUDED_)
#define AFX_UNIPATSYSCARD_H__402568D5_E278_11D1_A88F_00000100782D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////

class CCardGame;
class CStack;
class CCard;
class CGameRules;
class CEngine;
struct CCardLayout;

enum estacktype { ST_NUL, ST_FND, ST_TAB, ST_DECK, ST_WASTE, ST_DRAG, ST_HAND, 
		  ST_DISCARD, ST_RESERVE, ST_SIZE };
enum esuit { SU_NUL, SU_CLUB, SU_DIAMOND, SU_HEART, SU_SPADE, SU_SIZE };
enum erank { RK_NUL, RK_ACE, RK_2, RK_3, RK_4, RK_5, RK_6, RK_7, RK_8, 
	     RK_9, RK_10, RK_JACK, RK_QUEEN, RK_KING, RK_SIZE };
enum ecolour { CO_NUL, CO_RED, CO_BLACK, CO_SIZE };
enum eflags { CF_NUL = 0, CF_HIDDEN = 1, CF_VISIBLE = 2, CF_FACEDOWN = 4, CF_FACEUP = 8 };
enum eseq  { SQ_NUL = 0, SQ_ASCENDING = 1, SQ_DESCENDING = 2, SQ_ALTCOLOUR = 4, SQ_SAMESUIT = 8, SQ_SAMERANK = 16 };
enum ehint { HT_NUL = 0, HT_INIT = 0x1000, HT_STACK = 0x2000 };
enum erelop { ROP_NUL, EQ, NE, GT, GE, LT, LE, ROP_SIZE };
enum ecardop { COP_NUL, RANK_EQ, RANK_NE, SUIT_EQ, SUIT_NE, RED_EQ, RED_NE, BLACK_EQ, BLACK_NE, COP_SIZE };
enum emovetype { MT_NUL, MT_BEGIN, MT_END, MT_DEAL, MT_POSTDROP, MT_MOVECARDS, MT_MOVESTACK, MT_SETFLAGS };
enum eplace { PL_NUL, PL_SAMEROW, PL_SAMECOL, PL_ADJACENT };

const unsigned hintArgMask = 0xfff;

const int MAXRANK = 13;
const int MAXSUIT = 4;
const int ALLCARDS = 9999;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// class of card - one instance for each actual card

// responsible for behaviour of a card independent of how it is viewed

class CCard : public CObject {
// Attributes
protected:
   int m_suit;			// 1..
   int m_rank;			// 1..
   bool m_faceup;		// TRUE if card face visible
   bool m_visible;		// TRUE if edge of card visible when covered
   static char* suitText [];	// array of names of suits
   static char* rankText [];	// array of names of ranks

// Constructors
public:
   CCard ()			{ init (0, 0); }
   CCard (int ord)		{ init ((ord-1) / MAXRANK + 1, (ord-1) % MAXRANK + 1); }
   CCard (esuit s, erank r)	{ init (s, r); }

// Operations
public:
   esuit Suit () const		{ return esuit (m_suit); }
   erank Rank () const		{ return erank (m_rank); }
   char SuitChar () const	{ return "?cdhs"[m_suit]; }
   ecolour Colour () const	{ return m_suit == SU_DIAMOND || m_suit == SU_HEART ? CO_RED : CO_BLACK; }
   char* SuitText () const	{ return suitText [m_suit - 1]; }
   char* RankText () const	{ return rankText [m_rank - 1]; }
   int Ord () const		{ return MAXRANK * (m_suit-1) + m_rank; }
   bool IsRed () const		{ return m_suit == SU_DIAMOND 	|| m_suit == SU_HEART; }
   bool IsBlack () const	{ return m_suit == SU_CLUB 	|| m_suit == SU_SPADE; }
   bool SameColour (CCard *pc) 	{ return IsRed () == pc->IsRed (); }
   bool AltColour (CCard *pc) 	{ return IsRed () == pc->IsBlack (); }
   bool SameSuit (CCard *pc) 	{ return Suit () == pc->Suit (); }
   bool SameRank (CCard *pc) 	{ return Rank () == pc->Rank (); }
   bool IsSucc (CCard *pc) 	{ return int (pc->Rank ()) == int (Rank ()) + 1; }
   bool IsPred (CCard *pc) 	{ return int (pc->Rank ()) == int (Rank ()) - 1; }
   bool TestSeq (CCard *pc, eseq flags);
   void SetFlags (eflags flags);
   bool TestFlags (eflags flags);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
// implementation
private:
   void init (int s, int r) 
      { m_suit = s; m_rank = r; m_faceup = FALSE; m_visible = FALSE; AssertValid (); }

};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

class CStack : public CObject {
// Attributes
public:
   CCardGame *m_cardgame;	// parent is the game
   estacktype m_type;		// type of stack
   CTypedPtrList<CObList, CCard*> m_cardlist;
   int m_ord;			// ordinal with stacks of same type
   int m_row;                   // row no (1..)
   int m_col;                   // column no (1..)
   static char* nameText [];	// array of names 

// constructors
public:
   CStack (CCardGame* cardgame, estacktype type = ST_NUL, int ord = 0, int row = 0, int col = 0) 
      { m_cardgame = cardgame; m_type = type; m_ord = ord; m_row = row; m_col = col; }
   virtual ~CStack ();

// Operations
public:
   bool IsEmpty ()       { return !!m_cardlist.IsEmpty (); }
   int GetCount ()       { return m_cardlist.GetCount (); }
   char* Text () const	 { return nameText [m_type - 1]; }
   estacktype Type ()	 { return m_type; }
   void RemoveAll ()	 { m_cardlist.RemoveAll (); }
   CCard *HeadCard ()     { return m_cardlist.IsEmpty () ? NULL : m_cardlist.GetHead (); }
   CCard *TailCard ()  { return m_cardlist.IsEmpty () ? NULL : m_cardlist.GetTail (); }
   CCard *NewCard (esuit s, erank r, eflags flags = CF_VISIBLE);
   void Shuffle ();
   void MoveCards (CStack* ps, int n = 1, eflags flags = CF_NUL);
   bool TestSeq (eseq flags, int num = 1);
   void SetFlags (eflags flags, int num = 1);
   bool TestFlags (eflags flags, int num = 1);
   int Sum (int num = ALLCARDS);
   bool TestPlace(eplace pl, CStack *ps);
   void MoveStack (CStack* ps, int n);
   void CopyStack (CStack* ps, int n);
//   void JoinStack (CStack* ps);
   void NotifyChange (int num = 0);
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////
// layout of cards for a game - one per stack type
// all locations and dimensions are scaled on the basis of a card = 10x10
struct CCardLayout {
   estacktype type;		       // type of stacks in this section
   int nstack;			       // number of stacks (in order)
   //int ndown;			       // number of cards to deal face down
   //int nup;			       // number of cards to deal face up
   SIZE  card_offset;		       // offset from one card to the next (scaled)
   POINT stack_loc;		       // where this stack of cards is (scaled)
   SIZE  max_size;		       // maximum size (scaled)
   SIZE  next_stack;		       // offset to next stack (scaled)
};

/////////////////////////////////////////////////////////////////////////////
// CCardMove - a move made in the course of the game
//
class CCardMove : public CObject {
public:
   emovetype type;                     // type of move
   estacktype desttype;                // destination stack
   int destord;
   estacktype srctype;                 // source stack
   int srcord;
   int num;
   eflags flags;
   CCardMove() {}
   CCardMove(emovetype type, CStack* pdest, CStack* psrc, int num = 0, eflags flags = CF_NUL);
   virtual void Serialize(CArchive& ar);   // overridden for document i/o
};

/////////////////////////////////////////////////////////////////////////////

class CCardGame : public CObject {
// Attributes
public:
   CDocument* m_document;
   CTypedPtrList<CObList, CStack*> m_stacks;
//   CTypedPtrArray<CObArray, CGameRules*> m_games;
   int m_maxstack [ST_SIZE];
   CGameRules* m_rules;
   bool m_inited;
   int m_gameno;		       // game count 1=first
   int m_dealno;		       // deal count 1=first
   int m_roundno;		       // cout of rounds 1=first
   //bool m_automove;
   //bool m_bestmove_sclick;
   CString m_gametitle;
   int m_seed;
//   CEngine* m_engine;
   CTypedPtrList<CObList, CCardMove*> m_moves;
   CTypedPtrList<CObList, CCardMove*> m_redos;
   bool m_undoisopen;

// constructors
public:
   CCardGame (CDocument* parent);
   virtual ~CCardGame ();

// properties
   CCardLayout* CardLayout ();
   bool GetTitle (int num, CString& title);
   virtual int Finished ();
   int DealNo();
   int RoundNo();

// Operations
public:
//   CCardLayout* CardLayout () { return m_rules->CardLayout (); }
   void Clear();
   void NewGame (CString game, int seed);
   void NewGame () { NextGame(0); }
   void NextGame (int seed = 0);
   void RestartGame ();
   void CheckFinished ();
   virtual void BestMove (CStack* ps, int num = 0);
   void NewStacks (estacktype type, int num = 1, CCardLayout* layout = NULL);
   CStack* Stack (estacktype type, int ord = 0);
   void DealCards (CStack* pst, int ndown = 1, int nup = 1, 
      eflags flags = CF_VISIBLE);
   //void DealCards (estacktype type, int ord = 0, int ndown = 1, int nup = 1, 
   //   eflags flags = CF_VISIBLE);
   void DealCard (CStack *psx, esuit s, erank r);
   void NotifyChange (unsigned type, CObject* obj);
   void NotifyChangeStack (int nstacks, CStack* stack);
   void DoDeal();
   int DoPostDrop(CStack *ps);
   void DoMoveCards(CStack *psrc, CStack *pdest, int num, eflags flags);
   void DoMoveStack(CStack *psrc, CStack *pdest, int num = 1);
   void DoSetFlags(CStack *pdest, eflags flags, int num = 1);
   bool CanUndo();
   bool CanRedo();
   void Undo();
   void Redo();
   int ValidTurnup(CStack* ps);
   int ValidSplit(CStack* ps, int num = 1);
   int ValidJoin(CStack* pss, CStack* ps);
   int ValidMove(CStack *psrc, CStack *pdest, int num = 1);
   bool ValidDeal () { return true; }
   void SetUndoBegin() { AddUndoableMove(MT_BEGIN); }
   void SetUndoEnd() { AddUndoableMove(MT_END); }
   void DoMove(CCardMove* pmove);

   virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
   virtual void AssertValid() const;
   virtual void Dump(CDumpContext& dc) const;
#endif

private:
   void Setup();
   void AddUndoableMove(emovetype mvt);
   void AddUndoableMove(CCardMove* move);
   void ClearUndo();
   void ReplayGame();
//   void MakeLayout ();
   void Deal ();
};

#endif // !defined(AFX_UNIPATSYSCARD_H__402568D5_E278_11D1_A88F_00000100782D__INCLUDED_)
