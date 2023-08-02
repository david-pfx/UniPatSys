// UniPatSysCard.cpp: implementation of the CCard class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <time.h>
#include "UniPatSys.h"
#include "UPSGameRules.h"
#include "UPSGameProps.h"
#include "CardDeckLib.h"
#include "UniPatSysCard.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CGameParams CGameParams::GameParams;

//////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CCardGame

//IMPLEMENT_DYNCREATE(CCardGame, CDocument)

/////////////////////////////////////////////////////////////////////////////
// CCardGame construction/destruction

CCardGame::CCardGame(CDocument* doc) {
   m_document = doc;
   m_gameno = 0;
   m_dealno = 0;
   m_roundno = 0;
   m_seed = 1;
   m_undoisopen = false;
   m_inited = false;
   CGameRules::Create(this);
   CCardDeckLib::Create();
   m_rules = CGameRules::GetGame(0);     // default
   m_gametitle = m_rules->GameParams()->title;
}

CCardGame::~CCardGame() { 
   Clear();
}

void CCardGame::Clear() {
   while (!m_stacks.IsEmpty())
      delete m_stacks.RemoveHead();
   m_dealno = 0;
   m_roundno = 0;
   memset(m_maxstack, 0, sizeof(m_maxstack));
   m_inited = false;
}

CCardLayout* CCardGame::CardLayout () { 
   return m_rules->CardLayout (); 
}

int CCardGame::DealNo() {
   return m_rules->GameParams()->maxdeal > 0 ? m_dealno > 0 : 0;
}

int CCardGame::RoundNo() {
   return m_rules->GameParams()->maxround > 0 ? m_roundno : 0;		      
}

/////////////////////////////////////////////////////////////////////////////
// CCardGame serialization

void CCardGame::Serialize(CArchive& ar) {
   CObject::Serialize(ar);
   if (ar.IsStoring()) {
      ar << m_dealno << m_roundno << m_inited << m_gametitle << m_seed
	 << m_moves.GetCount();
      POSITION pos = m_moves.GetHeadPosition();
      while (pos) {
         CCardMove* pmove = m_moves.GetNext(pos);
         pmove->Serialize(ar);
      }      
   } else {
      int nmoves;
      ar >> m_dealno >> m_roundno >> m_inited >> m_gametitle >> m_seed
	 >> nmoves;
      NewGame(m_gametitle, m_seed);
      for (int i = 0; i < nmoves; ++i) {
         CCardMove* pmove = new CCardMove();
         pmove->Serialize(ar);
         AddUndoableMove(pmove);
//         DoMove(pmove);
      }
      ReplayGame();
   }
}

/////////////////////////////////////////////////////////////////////////////
// CCardGame diagnostics

#ifdef _DEBUG
void CCardGame::AssertValid() const
{
   ASSERT(m_dealno >= 0);
   POSITION pos = m_stacks.GetHeadPosition();
   while (pos) {
      CStack *ps = m_stacks.GetNext(pos);
      ASSERT(ps->m_cardgame == this);
      ps->AssertValid();
   }
}

void CStack::AssertValid() const {
   ASSERT(m_type >= ST_NUL && m_type < ST_SIZE);
   ASSERT(m_ord >= 0);
   POSITION pos = m_cardlist.GetHeadPosition();
   while (pos) {
      CCard *pc = m_cardlist.GetNext(pos);
      pc->AssertValid();
   }
}

void CCard::AssertValid() const {
   ASSERT(Suit() > SU_NUL && Suit() < SU_SIZE 
        && Rank() > RK_NUL && Rank() < RK_SIZE);
   ASSERT(m_faceup == !!m_faceup);
   ASSERT(m_visible == !!m_visible);
}

void CCardGame::Dump(CDumpContext& dc) const
{
   dc << "Deal " << m_dealno << "\n";
   POSITION pos = m_stacks.GetHeadPosition();
   while (pos) {
      CStack *ps = m_stacks.GetNext(pos);
      ps->Dump(dc);
   }
}

void CStack::Dump(CDumpContext& dc) const
{
   CObject::Dump(dc);
   dc << "Type " << int(m_type)
      << " ord " << m_ord
      << "\n";
   POSITION pos = m_cardlist.GetHeadPosition();
   while (pos) {
      CCard *pc = m_cardlist.GetNext(pos);
      pc->Dump(dc);
   }
}

void CCard::Dump(CDumpContext& dc) const
{
   CObject::Dump(dc);
   dc << "Suit " << int(Suit())
      << " rank " << int(Rank())
      << " faceup " << m_faceup 
      << " visible " << m_visible 
      << "\n";
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCardGame implementation functions

void CCardGame::AddUndoableMove(emovetype mvt) { 
   AddUndoableMove(new CCardMove(mvt, NULL, NULL, 0, CF_NUL));
}

void CCardGame::AddUndoableMove(CCardMove* pmove) {
   CCardMove* pmv;
   switch (pmove->type) {
   case MT_BEGIN:
      _ASSERTE(!m_undoisopen);
      m_undoisopen = true;
      m_moves.AddTail(pmove);
      break;
   case MT_END:
      _ASSERTE(m_undoisopen);
      pmv = m_moves.GetTail();
      if (pmv->type == MT_BEGIN)
         m_moves.RemoveTail();      // remove empty begin-end pair
      else {
         m_moves.AddTail(pmove);
         m_redos.RemoveAll();       // clear redo history
      }
      m_undoisopen = false;
      break;
   default:
      _ASSERTE(m_undoisopen);
      m_moves.AddTail(pmove);
   }
}

bool CCardGame::CanUndo() {
   return m_moves.GetCount() > 0;
}

bool CCardGame::CanRedo() {
   return m_redos.GetCount() > 0;
}

void CCardGame::Undo() {
   _ASSERTE(!m_undoisopen);
   if (CanUndo()) {
      CCardMove* pmv = m_moves.GetTail();
      _ASSERTE(pmv->type == MT_END);
      do {
         pmv = m_moves.RemoveTail();
         m_redos.AddHead(pmv);
      } while (pmv->type != MT_BEGIN);
      ReplayGame();
   }
}

void CCardGame::Redo() { 
   _ASSERTE(!m_undoisopen);
   if (CanRedo()) {
      CCardMove* pmv = m_redos.GetHead();
      _ASSERTE(pmv->type == MT_BEGIN);
      do {
         pmv = m_redos.RemoveHead();
         m_moves.AddTail(pmv);
      } while (pmv->type != MT_END);
      ReplayGame();
   }
}

void CCardGame::ReplayGame() {
   Setup();
   POSITION pos = m_moves.GetHeadPosition();
   while (pos) {
      CCardMove* pmove = m_moves.GetNext(pos);
      DoMove(pmove);
   }      
   NotifyChange(HT_INIT, NULL);
}

void CCardGame::ClearUndo() {
   m_moves.RemoveAll();
   m_redos.RemoveAll();
}

// get the title string for the game of this number
bool CCardGame::GetTitle(int num, CString& title) {
   if (num >= 0 && num < CGameRules::MaxGame()) {
      title = CGameRules::GetGame(num)->Title();
//   if (CGameRules::Exists(num)) {
//      title = CGameRules::Title(num);
      return true;
   } else
      return false;
}

// create a new card game given name and seed(or 0)
void CCardGame::NewGame(CString game, int seed) {
   m_rules = CGameRules::FindGame(game);
   if (m_rules) {
      m_gametitle = game;
      m_gameno = 0;
      NextGame(seed);
   } else {
      CString css;
      css.Format("Game '%s' does not exist.  Please choose another!", 
	 (const char*)game);
      AfxGetMainWnd()->MessageBox(css, NULL, MB_ICONERROR);
   }
}

// stop this game and start a new one of the same type
void CCardGame::NextGame(int seed) {
   if (seed)
      m_seed = seed;
   else ++m_seed;
   m_gameno++;
   RestartGame();
}

// restart this game
void CCardGame::RestartGame() {
   ClearUndo();
   Setup();
   NotifyChange(HT_INIT, NULL);
}

// set up for playing a game
void CCardGame::Setup() {
   Clear();
   srand(m_seed);
   m_rules->MakeLayout();
   NewStacks(ST_DRAG, 1);
   Stack(ST_DECK, 1)->Shuffle();
   CString s;
   s.Format("%s:%d", m_rules->GameParams()->title, m_seed);
   m_document->SetTitle((const char*)s);
   CGameProps::GameProps._game_name = m_gametitle;
   CGameProps::GameProps._seed = m_seed;
   m_inited = true;
}

// create NUM new stacks of given type and number them
void CCardGame::NewStacks(estacktype type, int num, CCardLayout* layout) {
   int row = m_maxstack [type] == 0 ? 1 : Stack(type, m_maxstack [type])->m_row + 1;
   for (int ord = 1; ord <= num; ord++) {
      CStack *ps = new CStack(this, type, m_maxstack [type] + ord, row, ord);
      m_stacks.AddTail(ps);
      ps->NotifyChange(0);
   }
   m_maxstack [type] += num;
}

 // find a stack given its type and ordinal(0=any)
CStack* CCardGame::Stack(estacktype type, int ord) {
   POSITION pos = m_stacks.GetHeadPosition();
   while (pos) {
      CStack *ps = m_stacks.GetNext(pos);
      if (type == ps->m_type &&(ord == 0 || ord == ps->m_ord))
         return ps;
   }
   return FALSE;
}

// deal N cards down and M up from deck to specified stack
void CCardGame::DealCards(CStack *psx, int ndown, int nup, eflags flags) {
   CStack *psd = Stack(ST_DECK);
   _ASSERTE(psd && psx);
   psx->MoveCards(psd, ndown,(eflags)(flags|CF_FACEDOWN));
   psx->MoveCards(psd, nup,(eflags)(flags|CF_FACEUP));
   psd->NotifyChange(1);
   psx->NotifyChange(ndown+nup);
}

// find single card and deal it face-up to top of to given stack
void CCardGame::DealCard(CStack *psx, esuit s, erank r) {
   CStack *psd = Stack(ST_DECK);
   _ASSERTE(psd && psx);
   POSITION pos = psd->m_cardlist.GetHeadPosition();
   for (int n = 1; pos; n++) {
      CCard *pc = psd->m_cardlist.GetAt(pos);
      if ((s == 0 || s == pc->Suit()) &&(r == 0 || r == pc->Rank())) {
	 psd->m_cardlist.RemoveAt(pos);
	 pc->SetFlags(CF_FACEUP);
	 psx->m_cardlist.AddHead(pc);
	 psd->NotifyChange(n);
	 psx->NotifyChange(1);
	 break;
      }
      psd->m_cardlist.GetNext(pos);
   }
}

// notify views of changes of N cards in a stack(0=all)
void CCardGame::NotifyChangeStack(int ncards, CStack* stack) {
   if (m_inited)
      m_document->UpdateAllViews(NULL, HT_STACK + ncards, stack);
}

void CCardGame::NotifyChange(unsigned type, CObject* obj) {
   if (m_inited)
      m_document->UpdateAllViews(NULL, type, obj);
}

// check if finished, making any automatic moves first
void CCardGame::CheckFinished() {
   if (CGameProps::GameProps._automove && 
       m_rules->GameParams()->autodest != ST_NUL) {
      CStack *pss;	   // source stack
      CStack *psd;	   // destination stack
//      CStack *pst = Stack(ST_DRAG);	 // temp stack
//      _ASSERTE(pst && pst->IsEmpty());
      bool moved;
      do {
	 moved = false;
	 for (int j = 1; j < ST_SIZE; j++) {
	    for (int i = 1; !moved; i++) {
	       pss = Stack((estacktype)j, i);
	       if (!pss)	       // no more of this type of stack
		  break;
	       for (int n = 1; n <= pss->GetCount(); n++) {
		  if (!ValidSplit(pss, n))
		     continue;
//		  pst->CopyStack(pss, n);
		  for (int j = 1; !moved; j++) {
		     psd = Stack(m_rules->GameParams()->autodest, j);
		     if (!psd)	       // no more of this type of stack
			break;
//		     if (ValidJoin(psd, pst))
		     if (ValidMove(psd, pss, n))
			moved = true;
		  }
//		  pst->RemoveAll();
                  if (moved) {
		     DoMoveStack(psd, pss, n);
                     //DoPostDrop(psd);
                  }
	       }
	    }
	 }
      } while (moved);
   }
   if (Finished())
      AfxGetMainWnd()->MessageBox("Congratulations!  You win!");
}

// best available move
void CCardGame::BestMove(CStack* pss, int num) {
   _ASSERTE(pss && num >= 0);
   switch(pss->m_type) {
   case ST_DECK:
      DoDeal();
      break;
   default:
      if (num >= 1 && ValidSplit(pss, num)) {
//	 CStack *pst = Stack(ST_DRAG);	 // temp stack
//	 _ASSERTE(pst);
//	 pst->CopyStack(pss, num);
	 int val, valb = 0;
	 CStack *psb;
	 for (int j = 1; j < ST_SIZE; j++) {
//	 for (int j = 1; j < ST_SIZE && pst; j++) {
	    for (int i = 1; ; i++) {
	       CStack *psd = Stack((estacktype)j, i);
	       if (!psd)	       // no more of this type of stack
		  break;
	       if (psd != pss &&(!!(val = ValidMove(psd, pss, num)))) {
//	       if (psd != pss &&(!!(val = ValidJoin(psd, pst)))) {
		  if (val > valb) {
		     valb = val;
		     psb = psd;
		  }
	       }
	    }
	 }
//	 pst->RemoveAll();
         if (valb) {
	    DoMoveStack(psb, pss, num);      // found a best move
            //DoPostDrop(psb);
         }
      }
      break;
   }
}

// it's finished when all the cards are on foundations or in the discard pile
int CCardGame::Finished() {
   int ncards = 0;
   CStack *psd;
   estacktype stacks [] = { ST_FND, ST_DISCARD, ST_NUL };
   for (int j = 0; stacks [j] != ST_NUL; j++) {
      for (int i = 1; psd = Stack(stacks [j], i); i++) {
	 ncards += psd->GetCount();
      }
   }
   return(ncards == m_rules->GameParams()->ncards) ? 1 : 0;
}

// deal another round of cards
void CCardGame::DoDeal() {
   AddUndoableMove(MT_DEAL);
   Deal();
   this->m_document->SetModifiedFlag();
}

void CCardGame::DoMoveCards(CStack *pdest, CStack *psrc, int num, eflags flags) {
   AddUndoableMove(new CCardMove(MT_MOVECARDS, pdest, psrc, num, flags));
   pdest->MoveCards(psrc, num, flags);
   this->m_document->SetModifiedFlag();
}

void CCardGame::DoMoveStack(CStack *pdest, CStack *psrc, int num) {
   AddUndoableMove(new CCardMove(MT_MOVESTACK, pdest, psrc, num));
   pdest->MoveStack(psrc, num);
   AddUndoableMove(new CCardMove(MT_POSTDROP, pdest, NULL, 0));
   DoPostDrop(pdest);
   this->m_document->SetModifiedFlag();
}

void CCardGame::DoSetFlags(CStack *pdest, eflags flags, int num) {
   AddUndoableMove(new CCardMove(MT_SETFLAGS, pdest, NULL, num, flags));
   pdest->SetFlags(flags, num);
   this->m_document->SetModifiedFlag();
}

void CCardGame::DoMove(CCardMove* pmove) {
   switch(pmove->type) {
   case MT_NUL:
   case MT_BEGIN:
   case MT_END:
      break;
   case MT_DEAL:
      Deal();
      break;
   case MT_POSTDROP:
      DoPostDrop(Stack(pmove->desttype, pmove->destord));
      break;
   case MT_MOVECARDS:
      Stack(pmove->desttype, pmove->destord)->MoveCards(
         Stack(pmove->srctype, pmove->srcord), pmove->num, pmove->flags);
      break;
   case MT_MOVESTACK:
      Stack(pmove->desttype, pmove->destord)->MoveStack(
         Stack(pmove->srctype, pmove->srcord), pmove->num);
      break;
   case MT_SETFLAGS:
      Stack(pmove->desttype, pmove->destord)->SetFlags(pmove->flags, pmove->num);
      break;
   default: ;
      _ASSERTE(FALSE);
   }
}

void CCardGame::Deal() { 
   int maxdeal = m_rules->GameParams()->maxdeal;
   if (m_dealno == 0) {
      m_dealno++;
      m_rules->DealLayout();
   } else if (m_roundno < m_rules->GameParams()->maxround && Stack(ST_DECK)->GetCount() > 0) {
      m_roundno++;
      m_rules->DealRound();
   } else if (m_roundno < m_rules->GameParams()->maxdeal) {
      m_roundno = 0;
      m_dealno++;
      m_rules->DealRedeal();
//   } else if (Stack(ST_DECK)->GetCount() == 0) {
//      NextGame();
   }
}

//// test whether allowed to drop this stack onto that one
//int CCardGame::ValidJoin(CStack* psd, CStack* pss) {
//   return m_rules->ValidJoin(psd, pss);
//}

// test whether allowed to split this stack at this point
int CCardGame::ValidSplit(CStack* ps, int num) {
   return m_rules->ValidSplit(ps, num);
}

// test whether allowed to turn up top card on this stack
int CCardGame::ValidTurnup(CStack* ps) {
   return m_rules->ValidTurnup(ps);
}

// test whether allowed to make this particular move
int CCardGame::ValidMove(CStack *psd, CStack *pss, int num) {
   if (!(pss != psd && num > 0 && m_rules->ValidSplit(pss, num)))
      return 0;
   CStack *pst = Stack(ST_DRAG);	 // temp stack
   _ASSERTE(pst && pst->IsEmpty());
   pst->CopyStack(pss, num);
   int val = m_rules->ValidJoin(psd, pst);
   pst->RemoveAll();
   return val;
}

// perform any post-drop action
int CCardGame::DoPostDrop(CStack *ps) {
   return m_rules->PostDrop(ps);
}

/////////////////////////////////////////////////////////////////////////////
//
CCardMove::CCardMove(emovetype type, CStack* pdest, CStack* psrc, int num, eflags flags) {
   this->type = type;
   this->desttype = (pdest) ? pdest->m_type : ST_NUL;
   this->destord = (pdest) ? pdest->m_ord : 0;
   this->srctype = (psrc) ? psrc->m_type : ST_NUL;
   this->srcord = (psrc) ? psrc->m_ord : 0;
   this->num = num;
   this->flags = flags;
}

void CCardMove::Serialize(CArchive& ar) {
   CObject::Serialize(ar);
   if (ar.IsStoring()) {
      ar << type << desttype << destord << srctype 
         << srcord << num << flags;
   } else {
      ar >> (int&)type >> (int&)desttype >> destord >> (int&)srctype 
         >> srcord >> num >> (int&)flags;
   }
}

/////////////////////////////////////////////////////////////////////////////
// CStack implementation functions

char* CStack::nameText [] = { 
   "Foundation", "Tableau", "Deck", "Waste", "Drag", "Hand", "Discard", "Reserve" 
};

// destructor - destroy the cards
CStack::~CStack() {
   while (!m_cardlist.IsEmpty()) 
      delete m_cardlist.RemoveHead();
}
   
CCard *CStack::NewCard(esuit s, erank r, eflags flags) {
CCard *pc = new CCard(s, r);
   m_cardlist.AddHead(pc);
   pc->SetFlags(flags);
   NotifyChange(1);
   return pc;
}

void CStack::NotifyChange(int num) {
   m_cardgame->NotifyChangeStack(num, this);
}

// shuffle the order of cards in a stack
void CStack::Shuffle() {
   int numcards = GetCount();
   CCard** pc = new CCard* [numcards];
   int n;
   for (n = 0; n < numcards; n++)
      pc [n] = m_cardlist.RemoveHead();
   for (n = 0; n < numcards; n++) {
      int nr =((rand() & 0xfff) * numcards) / 0x1000;
      _ASSERTE(nr >= 0 && nr < numcards);
      CCard *pctemp = pc [n];
      pc [n] = pc [nr];
      pc [nr] = pctemp;
   }
   for (n = 0; n < numcards; n++) {
      ASSERT_VALID(pc [n]);
      m_cardlist.AddHead(pc [n]);
   }
   _ASSERTE(m_cardlist.GetCount() == numcards);
   delete [] pc;
}

// move cards from supplied stack to this one, by ones, setting flags
void CStack::MoveCards(CStack *ps, int num, eflags flags) {
   int n = num = min(num, ps->GetCount());
   while (n-- > 0) {
      _ASSERTE(!ps->IsEmpty());
      CCard *pc = ps->m_cardlist.RemoveHead();
      m_cardlist.AddHead(pc);
      pc->SetFlags(flags);
   }
   ps->NotifyChange(1);
   NotifyChange(num);
}

// move front section of supplied stack to this one
void CStack::MoveStack(CStack *ps, int num) {
   int n = num = min(num, ps->GetCount());
   CCard *pc;
   POSITION pos = NULL;		       // position for insertion(NULL = head)
   while (n-- > 0 && !ps->IsEmpty()) {
      pc = ps->m_cardlist.RemoveHead();
      if (pos == NULL)
	 pos = m_cardlist.AddHead(pc);
      else
	 pos = m_cardlist.InsertAfter(pos, pc);
   } 
   ps->NotifyChange(num);
   NotifyChange(num);
}

// copy front section of supplied stack to this one(used for dragging)
void CStack::CopyStack(CStack *ps, int num) {
   int n = num = min(num, ps->GetCount());
   POSITION poss = ps->m_cardlist.GetHeadPosition(); // position for retrieval
   POSITION pos = NULL;		       // position for insertion(NULL = head)
   while (n-- > 0) {
      _ASSERTE(poss);
      CCard *pc = ps->m_cardlist.GetNext(poss);
      if (pos == NULL)
	 pos = m_cardlist.AddHead(pc);
      else
	 pos = m_cardlist.InsertAfter(pos, pc);
   } 
   m_col = ps->m_col;
   m_row = ps->m_row;
   NotifyChange(num);
}

// make some cards visible or faceup
void CStack::SetFlags(eflags flags, int num) {
   int n = num = min(num, GetCount());
   POSITION pos = m_cardlist.GetHeadPosition();
   while (pos && n-- > 0) {
      m_cardlist.GetNext(pos)->SetFlags(flags);
   }
   NotifyChange(num);
}

// test whether some flags are set
bool CStack::TestFlags(eflags flags, int num) {
   num = min(num, GetCount());
   POSITION pos = m_cardlist.GetHeadPosition();
   while (pos && num-- > 0) {
      if (!m_cardlist.GetNext(pos)->TestFlags(flags))
         return FALSE;
   }
   return TRUE;
}

// add up the card values
int CStack::Sum(int num) {
   num = min(num, GetCount());
   int sum = 0;
   POSITION pos = m_cardlist.GetHeadPosition();
   while (pos && num-- > 0) {
      sum += m_cardlist.GetNext(pos)->Rank();
   }
   return sum;
}

// test whether part of stack conforms to a sequence
bool CStack::TestSeq(eseq flags, int num) {
   num = min(num, GetCount());
   POSITION pos = m_cardlist.GetHeadPosition();
   if (!pos)
      return TRUE;
   CCard *pc1 = m_cardlist.GetNext(pos);
   while (pos && num-- > 1) {
      CCard *pc2 = m_cardlist.GetNext(pos);
      if (pc2 && !pc1->TestSeq(pc2, flags))
         return FALSE;
      pc1 = pc2;
   }
   return TRUE;
}

enum eloctype { LT_NUL, LT_SAMEROW, LT_SAMECOL, LT_ADJACENT };

// test another stack is in the right place relative to this one
bool CStack::TestPlace(eplace place, CStack *ps) {
   switch(place) {
   case PL_SAMEROW:  return m_row == ps->m_row;
   case PL_SAMECOL:  return m_col == ps->m_col;
   case PL_ADJACENT: return abs(m_row - ps->m_row) <= 1 &&
                            abs(m_col - ps->m_col) <= 1;
   default:  _ASSERTE(FALSE);
   }
   return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CCard implementation functions

char* CCard::suitText [] = {
   "Club","Diamond","Heart","Spade"
};

char* CCard::rankText [] = {
   "Ace","Two","Three","Four","Five","Six",
   "Seven","Eight","Nine","Ten","Jack","Queen","King" 
};

// make some cards visible or faceup
void CCard::SetFlags(eflags flags) {
   if (flags & CF_HIDDEN)   m_visible = FALSE;
   if (flags & CF_VISIBLE)  m_visible = TRUE;
   if (flags & CF_FACEDOWN) m_faceup = FALSE;
   if (flags & CF_FACEUP)   m_faceup = TRUE;
}

// test if card is visible or faceup
bool CCard::TestFlags(eflags flags) {
   if (flags & CF_HIDDEN)   if (m_visible) return FALSE;
   if (flags & CF_VISIBLE)  if (!m_visible) return FALSE;
   if (flags & CF_FACEDOWN) if (m_faceup) return FALSE;
   if (flags & CF_FACEUP)   if (!m_faceup) return FALSE;
   return TRUE;
}

// test whether another card conforms to a sequence(must be faceup)
bool CCard::TestSeq(CCard *pc, eseq flags) {
   ASSERT(pc);
   if (!(TestFlags(CF_FACEUP) && pc->TestFlags(CF_FACEUP))) return FALSE;
   if (flags & SQ_ASCENDING)  if (!IsSucc(pc)) return FALSE;
   if (flags & SQ_DESCENDING) if (!IsPred(pc)) return FALSE;
   if (flags & SQ_ALTCOLOUR)  if (!AltColour(pc)) return FALSE;
   if (flags & SQ_SAMESUIT)   if (!SameSuit(pc)) return FALSE;
   if (flags & SQ_SAMERANK)   if (!SameRank(pc)) return FALSE;
   return TRUE;
}

// end of UniPatSysCard.cpp