// UPSGameRules.cpp: rules for UPS card games
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UniPatSys.h"
#include "UniPatSysCard.h"
#include "Engine.h"
#include "UPSGameRules.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CGameRules implementation functions

// statics
CCardGame* CGameRules::m_cardgame;
vector<CGameRules*> CGameRules::m_games;
CEngine* CGameRules::m_engine;

// ctor
CGameRules::CGameRules(CGameRulesInfo* info) {
   m_gameparams = info->m_gameparams;
   m_cardlayout = info->m_cardlayout;
   m_validjoin = info->m_validjoin;
   m_validsplit = info->m_validsplit;
   m_dealcards = info->m_dealcards;
}

// factory
void CGameRules::Create(CCardGame* game) {
   if (m_cardgame == NULL) {
      m_cardgame = game;
      m_engine = new CEngine(game);
      for (CGameRulesInfo* pgri = CGameRulesInfo::GameRulesInfo; 
	   pgri->m_gameparams != NULL; pgri++) {
	 m_games.push_back(new CGameRules(pgri));
      }
   }
}

// properties
CGameRules* CGameRules::FindGame(CString title) {
   for (int j = 0; j < MaxGame(); ++j)
      if (GetGame(j)->Title() == title)
         return GetGame(j);
   return NULL;
}

// operations
void CGameRules::MakeLayout() {
   for (CCardLayout* pl = CardLayout(); pl->type != ST_NUL; pl++)
      m_cardgame->NewStacks(pl->type, pl->nstack, pl);
   CStack *ps = m_cardgame->Stack(ST_DECK, 1);
   _ASSERTE(ps);
   int n = 0, ncards = m_gameparams->ncards;
   while (n < ncards) {
      for (int s = SU_CLUB; s <= SU_SPADE && n < ncards; s++) {
	 for (int r = RK_ACE; r <= RK_KING && n < ncards; r++) {
	    ps->NewCard(esuit(s), erank(r),(eflags)(CF_VISIBLE|CF_FACEDOWN));
	    ++n;
	 }
      }
   }
}

int CGameRules::DealLayout() {
   return m_engine->Evaluate(m_dealcards, 1, 1);
}

int CGameRules::DealRound() {
   return m_engine->Evaluate(m_dealcards, 1, 2);
}

int CGameRules::DealRedeal() {
   return m_engine->Evaluate(m_dealcards, 1, 3);
}

int CGameRules::PostDrop(CStack* psd) {
   return m_engine->Evaluate(m_dealcards, 2, 9, psd);
}

// test for valid join using destination and source stacks as arguments
int CGameRules::ValidJoin(CStack* psd, CStack* pss) {
   return m_engine->Evaluate(m_validjoin, 2, psd, pss);
}

// test for valid split using stack and number of cards as arguments
int CGameRules::ValidSplit(CStack* ps, int num) {
   return m_engine->Evaluate(m_validsplit, 2, ps, num);
}

bool CGameRules::ValidTurnup (CStack* ps) { 
   return ps->Type() != ST_DECK; 
}

/////////////////////////////////////////////////////////////////////////////
// game-specific rules

/////////////////////////////////////////////////////////////////////////////
// Klondike
/////////////////////////////////////////////////////////////////////////////

CGameParams gameParams_Klondike [] = {
   "Klondike",
   52,
   ST_NUL,
   9999, 9999           // rounds, deals and redeals
};

CCardLayout cardLayout_Klondike [] = {
//          num    offset     based loc    max size     next loc 
   ST_FND,   4,  {  0, 1 },  {  0, 0 },  {  10, 10 },  { 10, 0 },
   ST_TAB,   7,  {  0,10 },  {  0,10 },  {  10, 22 },  { 10, 0 },
   ST_DECK,  1,  {  2, 1 },  {  0,35 },  {   0,  0 },  {  0, 0 },
   ST_WASTE, 1,  { 10, 0 },  { 35,35 },  {  35, 10 },  {  0, 0 },
   ST_NUL,   0,  {  0, 0 },  {  0, 0 },  {   0,  0 },  {  0, 0 },
};

aCode dealCards_Klondike [] = {
   /* deal layout: to tableau, 0-1-2-3-4-5-6 down, all 1 up */
   XCOMPARE, XNUM1, EQ, 1,
      XDEAL, XSTK(ST_TAB,1), 0, 1,
      XDEAL, XSTK(ST_TAB,2), 1, 1,
      XDEAL, XSTK(ST_TAB,3), 2, 1,
      XDEAL, XSTK(ST_TAB,4), 3, 1,
      XDEAL, XSTK(ST_TAB,5), 4, 1,
      XDEAL, XSTK(ST_TAB,6), 5, 1,
      XDEAL, XSTK(ST_TAB,7), 6, 1,
   /* deal round: to waste, 3 up */
   XCOMPARE, XNUM1, EQ, 2,
      XDEAL, XSTK(ST_WASTE,1), 0, 3,
   /* gather for redeal: gather waste */
   XCOMPARE, XNUM1, EQ, 3,
      XGATHER, XSTK(ST_WASTE,1), ALLCARDS,
   XEND
};

aCode validJoin_Klondike [] = {
   /* drop a single card on empty foundation if it's an A */
   XSCOUNT, XSTK1, EQ, 0,
   XSTYPE, XSTK1, EQ, ST_FND,
   XSCOUNT, XSTK2, EQ, 1,
   XSTAIL, XSTK2, RANK_EQ, RK_ACE,
      XRETURN, 9,
   /* drop a stack on empty tableau if K at bottom */
   XSCOUNT, XSTK1, EQ, 0,
   XSTYPE, XSTK1, EQ, ST_TAB,
   XSTAIL, XSTK2, RANK_EQ, RK_KING,
      XRETURN, 7,
   /* drop a single card on non-empty foundation if same suit ascending */
   XSCOUNT, XSTK1, NE, 0,
   XSTYPE, XSTK1, EQ, ST_FND,
   XSCOUNT, XSTK2, EQ, 1,
   XSJOIN, XSTK1, SQ_SAMESUIT|SQ_ASCENDING, XSTK2,
      XRETURN, 5,
   /* drop a stack on non-empty tableau if alternating colour descending */
   XSCOUNT, XSTK1, NE, 0,
   XSTYPE, XSTK1, EQ, ST_TAB,
   XSJOIN, XSTK1, SQ_ALTCOLOUR|SQ_DESCENDING, XSTK2,
      XRETURN, 3,
   XEND
};

aCode validSplit_Klondike [] = {
   /* pick a stack from tableau if alternating colour descending */
   XSTYPE, XSTK1, EQ, ST_TAB,
   XSSEQ, XSTK1, SQ_ALTCOLOUR|SQ_ASCENDING, XSTK2,
      XRETURN, 1,
   /* pick a single card from waste */
   XSTYPE, XSTK1, EQ, ST_WASTE,
   XCOMPARE, XNUM2, EQ, 1,
      XRETURN, 1,
   XEND
};

/////////////////////////////////////////////////////////////////////////////
// Spider
/////////////////////////////////////////////////////////////////////////////

CGameParams gameParams_Spider [] = {
   "Spider",
   104,
   ST_NUL,
   9999, 0,         // rounds, deals and redeals
};

CCardLayout cardLayout_Spider [] = {
//          num    offset     based loc    max size     next loc 
   ST_FND,   4,  {  0, 0 },  {  0, 0 },  {   0,  0 },  { 10, 0 },
   ST_FND,   4,  {  0, 0 },  { 60, 0 },  {   0,  0 },  { 10, 0 },
   ST_TAB,  10,  {  0,10 },  {  0,10 },  {  10, 50 },  { 10, 0 },
   ST_DECK,  1,  {  0, 0 },  { 45, 0 },  {   0,  0 },  {  0, 0 },
   ST_NUL,   0,  {  0, 0 },  {  0, 0 },  {   0,  0 },  {  0, 0 },
};

aCode dealCards_Spider [] = {
   /* deal layout: to tableau, 4x5 6x4 down, all 1 up */
   XCOMPARE, XNUM1, EQ, 1,
      XDEAL, XSTK(ST_TAB, 1), 5, 1,
      XDEAL, XSTK(ST_TAB, 2), 5, 1,
      XDEAL, XSTK(ST_TAB, 3), 5, 1,
      XDEAL, XSTK(ST_TAB, 4), 5, 1,
      XDEAL, XSTK(ST_TAB, 5), 4, 1,
      XDEAL, XSTK(ST_TAB, 6), 4, 1,
      XDEAL, XSTK(ST_TAB, 7), 4, 1,
      XDEAL, XSTK(ST_TAB, 8), 4, 1,
      XDEAL, XSTK(ST_TAB, 9), 4, 1,
      XDEAL, XSTK(ST_TAB,10), 4, 1,
   /* deal round: to tableau, all 1 up */
   XCOMPARE, XNUM1, EQ, 2,
      XDEAL, XSTK(ST_TAB, 1), 0, 1,
      XDEAL, XSTK(ST_TAB, 2), 0, 1,
      XDEAL, XSTK(ST_TAB, 3), 0, 1,
      XDEAL, XSTK(ST_TAB, 4), 0, 1,
      XDEAL, XSTK(ST_TAB, 5), 0, 1,
      XDEAL, XSTK(ST_TAB, 6), 0, 1,
      XDEAL, XSTK(ST_TAB, 7), 0, 1,
      XDEAL, XSTK(ST_TAB, 8), 0, 1,
      XDEAL, XSTK(ST_TAB, 9), 0, 1,
      XDEAL, XSTK(ST_TAB,10), 0, 1,
   XEND
};

aCode validJoin_Spider [] = {
   /* drop a stack on empty foundation if it's 13 cards */
   XSCOUNT, XSTK1, EQ, 0,
   XSTYPE, XSTK1, EQ, ST_FND,
   XSCOUNT, XSTK2, EQ, 13,
      XRETURN, 9,
   /* drop a stack on empty tableau */
   XSCOUNT, XSTK1, EQ, 0,
   XSTYPE, XSTK1, EQ, ST_TAB,
      XRETURN, 2,
   /* drop a stack on non-empty tableau if it's same suit, descending */
   XSCOUNT, XSTK1, NE, 0,
   XSTYPE, XSTK1, EQ, ST_TAB,
   XSJOIN, XSTK1, SQ_SAMESUIT|SQ_DESCENDING, XSTK2,
      XRETURN, 6,
   /* drop a stack on non-empty tableau if it's descending */
   XSCOUNT, XSTK1, NE, 0,
   XSTYPE, XSTK1, EQ, ST_TAB,
   XSJOIN, XSTK1, SQ_DESCENDING, XSTK2,
      XRETURN, 4,
   XEND
};

aCode validSplit_Spider [] = {
   /* pick a stack from tableau if same suit ascending */
   XSTYPE, XSTK1, EQ, ST_TAB,
   XSSEQ, XSTK1, SQ_SAMESUIT|SQ_ASCENDING, XNUM2,
      XRETURN, 1,
   XEND
};

/////////////////////////////////////////////////////////////////////////////
// Cruel
/////////////////////////////////////////////////////////////////////////////

CGameParams gameParams_Cruel [] = {
   "Cruel",
   52,
   ST_NUL,
   0, 9999,            // rounds, deals and redeals
};

CCardLayout cardLayout_Cruel [] = {
//          num    offset     based loc    max size     next loc 
   ST_FND,   4,  {  0, 0 },  { 10, 0 },  {   0,  0 },  { 10, 0 },
   ST_TAB,   6,  {  0, 9 },  { 10,12 },  {  10, 18 },  { 10, 0 },
   ST_TAB,   6,  {  0, 9 },  { 10,32 },  {  10, 18 },  { 10, 0 },
   ST_DECK,  1,  {  0, 1 },  {  0,32 },  {   0,  0 },  {  0, 0 },
   ST_NUL,   0,  {  0, 0 },  {  0, 0 },  {   0,  0 },  {  0, 0 },
};

aCode dealCards_Cruel [] = {
   /* deal layout: to foundation: 4 aces, to tableau: 12x4 up */
   XCOMPARE, XNUM1, EQ, 1,
      XDEALC, XSTK(ST_FND,1), 0, RK_ACE,
      XDEALC, XSTK(ST_FND,2), 0, RK_ACE,
      XDEALC, XSTK(ST_FND,3), 0, RK_ACE,
      XDEALC, XSTK(ST_FND,4), 0, RK_ACE,
      XDEAL, XSTK(ST_TAB,1), 0, 4,
      XDEAL, XSTK(ST_TAB,2), 0, 4,
      XDEAL, XSTK(ST_TAB,3), 0, 4,
      XDEAL, XSTK(ST_TAB,4), 0, 4,
      XDEAL, XSTK(ST_TAB,5), 0, 4,
      XDEAL, XSTK(ST_TAB,6), 0, 4,
      XDEAL, XSTK(ST_TAB,7), 0, 4,
      XDEAL, XSTK(ST_TAB,8), 0, 4,
      XDEAL, XSTK(ST_TAB,9), 0, 4,
      XDEAL, XSTK(ST_TAB,10), 0, 4,
      XDEAL, XSTK(ST_TAB,11), 0, 4,
      XDEAL, XSTK(ST_TAB,12), 0, 4,
   /* gather for redeal: all tableau to deck; then to tableau: 12x4 up */
   XCOMPARE, XNUM1, EQ, 3,
      XMOVES, XSTK(ST_TAB, 1), XSTK(ST_TAB, 2), ALLCARDS,
      XMOVES, XSTK(ST_TAB, 2), XSTK(ST_TAB, 3), ALLCARDS,
      XMOVES, XSTK(ST_TAB, 3), XSTK(ST_TAB, 4), ALLCARDS,
      XMOVES, XSTK(ST_TAB, 4), XSTK(ST_TAB, 5), ALLCARDS,
      XMOVES, XSTK(ST_TAB, 5), XSTK(ST_TAB, 6), ALLCARDS,
      XMOVES, XSTK(ST_TAB, 6), XSTK(ST_TAB, 7), ALLCARDS,
      XMOVES, XSTK(ST_TAB, 7), XSTK(ST_TAB, 8), ALLCARDS,
      XMOVES, XSTK(ST_TAB, 8), XSTK(ST_TAB, 9), ALLCARDS,
      XMOVES, XSTK(ST_TAB, 9), XSTK(ST_TAB,10), ALLCARDS,
      XMOVES, XSTK(ST_TAB,10), XSTK(ST_TAB,11), ALLCARDS,
      XMOVES, XSTK(ST_TAB,11), XSTK(ST_TAB,12), ALLCARDS,
      XMOVES, XSTK(ST_TAB,12), XSTK(ST_DECK,1), ALLCARDS,
      XMOVES, XSTK(ST_DECK, 1), XSTK(ST_TAB, 1), 4,
      XMOVES, XSTK(ST_DECK, 1), XSTK(ST_TAB, 2), 4,
      XMOVES, XSTK(ST_DECK, 1), XSTK(ST_TAB, 3), 4,
      XMOVES, XSTK(ST_DECK, 1), XSTK(ST_TAB, 4), 4,
      XMOVES, XSTK(ST_DECK, 1), XSTK(ST_TAB, 5), 4,
      XMOVES, XSTK(ST_DECK, 1), XSTK(ST_TAB, 6), 4,
      XMOVES, XSTK(ST_DECK, 1), XSTK(ST_TAB, 7), 4,
      XMOVES, XSTK(ST_DECK, 1), XSTK(ST_TAB, 8), 4,
      XMOVES, XSTK(ST_DECK, 1), XSTK(ST_TAB, 9), 4,
      XMOVES, XSTK(ST_DECK, 1), XSTK(ST_TAB,10), 4,
      XMOVES, XSTK(ST_DECK, 1), XSTK(ST_TAB,11), 4,
      XMOVES, XSTK(ST_DECK, 1), XSTK(ST_TAB,12), 4,
   XEND
};

aCode validJoin_Cruel [] = {
   /* drop a stack on empty foundation if A at bottom */
   XSCOUNT, XSTK1, EQ, 0,
   XSTYPE, XSTK1, EQ, ST_FND,
   XSTAIL, XSTK2, RANK_EQ, RK_ACE,
      XRETURN, 9,
   /* drop a stack on non-empty foundation if same suit, ascending */
   XSCOUNT, XSTK1, NE, 0,
   XSTYPE, XSTK1, EQ, ST_FND,
   XSJOIN, XSTK1, SQ_SAMESUIT|SQ_ASCENDING, XSTK2,
      XRETURN, 5,
   /* drop a stack on non-empty tableau if same suit, descending */
   XSCOUNT, XSTK1, NE, 0,
   XSTYPE, XSTK1, EQ, ST_TAB,
   XSJOIN, XSTK1, SQ_SAMESUIT|SQ_DESCENDING, XSTK2,
      XRETURN, 3,
   XEND
};

aCode validSplit_Cruel [] = {
   /* pick any single card from tableau */
   XSTYPE, XSTK1, EQ, ST_TAB,
   XCOMPARE, XSTK2, EQ, 1,
//   XCOMPARE, XNUM2, EQ, 1,
      XRETURN, 1,
   XEND
};

/////////////////////////////////////////////////////////////////////////////
// Tower
/////////////////////////////////////////////////////////////////////////////

CGameParams gameParams_Tower [] = {
   "Tower",
   52,
   ST_FND,
   0, 0           // rounds, deals and redeals
};

CCardLayout cardLayout_Tower [] = {
//          num    offset     based loc    max size     next loc 
   ST_FND,   4,  {  0, 0 },  { 60, 0 },  {   0,  0 },  { 10, 0 },
   ST_HAND,  4,  {  0, 0 },  {  0, 0 },  {   0,  0 },  { 10, 0 },
   ST_TAB,  10,  {  0,15 },  {  0,12 },  {  10, 30 },  { 10, 0 },
   ST_DECK,  1,  {  0, 0 },  { 45, 0 },  {   0,  0 },  {  0, 0 },
   ST_NUL,   0,  {  0, 0 },  {  0, 0 },  {   0,  0 },  {  0, 0 },
};

aCode dealCards_Tower [] = {
   /* deal layout: to tableau, 10x5 up, to hand 2x1 up */
   XCOMPARE, XNUM1, EQ, 1,
      XDEAL, XSTK(ST_TAB,1), 0, 5,
      XDEAL, XSTK(ST_TAB,2), 0, 5,
      XDEAL, XSTK(ST_TAB,3), 0, 5,
      XDEAL, XSTK(ST_TAB,4), 0, 5,
      XDEAL, XSTK(ST_TAB,5), 0, 5,
      XDEAL, XSTK(ST_TAB,6), 0, 5,
      XDEAL, XSTK(ST_TAB,7), 0, 5,
      XDEAL, XSTK(ST_TAB,8), 0, 5,
      XDEAL, XSTK(ST_TAB,9), 0, 5,
      XDEAL, XSTK(ST_TAB,10), 0, 5,
      XDEAL, XSTK(ST_HAND,1), 0, 1,
      XDEAL, XSTK(ST_HAND,2), 0, 1,
   XEND
};

aCode validJoin_Tower [] = {
   /* drop a stack on empty foundation if bottom is A */
   XSCOUNT, XSTK1, EQ, 0,
   XSTYPE, XSTK1, EQ, ST_FND,
   XSTAIL, XSTK2, RANK_EQ, RK_ACE,
      XRETURN, 9,
   /* drop a stack on empty tableau if bottom is K */
   XSCOUNT, XSTK1, EQ, 0,
   XSTYPE, XSTK1, EQ, ST_TAB,
   XSTAIL, XSTK2, RANK_EQ, RK_KING,
      XRETURN, 4,
   /* drop a stack on empty hand */
   XSCOUNT, XSTK1, EQ, 0,
   XSTYPE, XSTK1, EQ, ST_HAND,
      XRETURN, 1,
   /* drop a stack on empty foundation if it's same suit ascending */
   XSCOUNT, XSTK1, NE, 0,
   XSTYPE, XSTK1, EQ, ST_FND,
   //XSCOUNT, XSTK2, EQ, 1,
   XSJOIN, XSTK1, SQ_SAMESUIT|SQ_ASCENDING, XSTK2,
      XRETURN, 5,
   /* drop a stack on non-empty tableau if it's same suit descending */
   XSCOUNT, XSTK1, NE, 0,
   XSTYPE, XSTK1, EQ, ST_TAB,
   XSJOIN, XSTK1, SQ_SAMESUIT|SQ_DESCENDING, XSTK2,
      XRETURN, 3,
   XEND
};

aCode validSplit_Tower [] = {
   /* pick a single card from tableau */
   XSTYPE, XSTK1, EQ, ST_TAB,
   XCOMPARE, XSTK2, EQ, 1,
   //XCOMPARE, XNUM2, EQ, 1,
      XRETURN, 1,
   /* pick a single card from hand */
   XSTYPE, XSTK1, EQ, ST_HAND,
   XCOMPARE, XSTK2, EQ, 1,
   //XCOMPARE, XNUM2, EQ, 1,
      XRETURN, 1,
   XEND
};

/////////////////////////////////////////////////////////////////////////////
// Addition
/////////////////////////////////////////////////////////////////////////////

CGameParams gameParams_Addition [] = {
   "Addition",
   52,
   ST_DISCARD,
   0, 9999,            // rounds, deals and redeals
};

CCardLayout cardLayout_Addition [] = {
//	      num    offset     based loc    max size     next loc 
   ST_TAB,     5,  {  0, 0 },  {  0, 0 },  {   0,  0 },  { 10, 0 },
   ST_TAB,     5,  {  0, 0 },  {  0,10 },  {   0,  0 },  { 10, 0 },
   ST_DECK,    1,  {  0, 0 },  {  0,30 },  {   0,  0 },  {  0, 0 },
   ST_HAND,    1,  { 10, 0 },  { 20,30 },  {   0,  0 },  {  0, 0 },
   ST_DISCARD, 1,  {  0, 0 },  { 40,30 },  {   0,  0 },  {  0, 0 },
   ST_NUL,     0,  {  0, 0 },  {  0, 0 },  {   0,  0 },  {  0, 0 },
};

aCode dealCards_Addition [] = {
   /* deal layout: to tableau 1-10, 1 up if empty */
   XSCOUNTS, XSTK(ST_TAB, 1), EQ, 0,
      XDEAL, XSTK(ST_TAB, 1), 0, 1,
   XSCOUNTS, XSTK(ST_TAB, 2), EQ, 0,
      XDEAL, XSTK(ST_TAB, 2), 0, 1,
   XSCOUNTS, XSTK(ST_TAB, 3), EQ, 0,
      XDEAL, XSTK(ST_TAB, 3), 0, 1,
   XSCOUNTS, XSTK(ST_TAB, 4), EQ, 0,
      XDEAL, XSTK(ST_TAB, 4), 0, 1,
   XSCOUNTS, XSTK(ST_TAB, 5), EQ, 0,
      XDEAL, XSTK(ST_TAB, 5), 0, 1,
   XSCOUNTS, XSTK(ST_TAB, 6), EQ, 0,
      XDEAL, XSTK(ST_TAB, 6), 0, 1,
   XSCOUNTS, XSTK(ST_TAB, 7), EQ, 0,
      XDEAL, XSTK(ST_TAB, 7), 0, 1,
   XSCOUNTS, XSTK(ST_TAB, 8), EQ, 0,
      XDEAL, XSTK(ST_TAB, 8), 0, 1,
   XSCOUNTS, XSTK(ST_TAB, 9), EQ, 0,
      XDEAL, XSTK(ST_TAB, 9), 0, 1,
   XSCOUNTS, XSTK(ST_TAB, 10), EQ, 0,
      XDEAL, XSTK(ST_TAB, 10), 0, 1,
   XEND
};

aCode validJoin_Addition [] = {
   /* drop a 1 or 2 card stack on discard if adds to 13 */
   XSTYPE, XSTK1, EQ, ST_DISCARD,
   XSCOUNT, XSTK2, LE, 2,
   XSSUM, XSTK2, EQ, 13,
      XRETURN, 9,
   /* drop single card on tableau if empty */
   XSTYPE, XSTK1, EQ, ST_TAB,
   XSCOUNT, XSTK1, EQ, 0,
   XSCOUNT, XSTK2, EQ, 1,
      XRETURN, 3,
   /* drop any single card on hand of 1 or 2 */
   XSTYPE, XSTK1, EQ, ST_HAND,
   XSCOUNT, XSTK1, LE, 1,  // new
   XSCOUNT, XSTK2, EQ, 1,
      XRETURN, 5,
   XEND
};

aCode validSplit_Addition [] = {
   /* pick from tableau: a single card */
   XSTYPE, XSTK1, EQ, ST_TAB,
   //XCOMPARE, XNUM2, EQ, 1,
      XRETURN, 1,
   /* pick from hand: a single card */
   XSTYPE, XSTK1, EQ, ST_HAND,
   //XCOMPARE, XSTK2, EQ, 1, // new
      XRETURN, 1,
   XEND,
   ///* pick from hand: stack adding to 13 */
   //XSTYPE, XSTK1, EQ, ST_HAND,
   //XSSUM, XSTK1, EQ, 13,
   //   XRETURN, 1,
   //XEND,
};

/////////////////////////////////////////////////////////////////////////////
// Fourteen
/////////////////////////////////////////////////////////////////////////////

CGameParams gameParams_Fourteen [] = {
   "Fourteen",
   52,
   ST_DISCARD,
   0, 0,          // rounds, deals and redeals
};

CCardLayout cardLayout_Fourteen [] = {
//	      num    offset     based loc    max size     next loc 
   ST_TAB,    12,  {  0,15 },  {  0, 0 },  {   0,  0 },  { 10, 0 },
   ST_DECK,    1,  {  0, 0 },  {  0,30 },  {   0,  0 },  {  0, 0 },
   ST_HAND,    1,  { 10, 0 },  { 20,30 },  {   0,  0 },  {  0, 0 },
   ST_DISCARD, 1,  {  0, 0 },  { 40,30 },  {   0,  0 },  {  0, 0 },
   ST_NUL,     0,  {  0, 0 },  {  0, 0 },  {   0,  0 },  {  0, 0 },
};

aCode dealCards_Fourteen [] = {
   XDEAL, XSTK(ST_TAB, 1), 0, 5,
   XDEAL, XSTK(ST_TAB, 2), 0, 5,
   XDEAL, XSTK(ST_TAB, 3), 0, 5,
   XDEAL, XSTK(ST_TAB, 4), 0, 5,
   XDEAL, XSTK(ST_TAB, 5), 0, 4,
   XDEAL, XSTK(ST_TAB, 6), 0, 4,
   XDEAL, XSTK(ST_TAB, 7), 0, 4,
   XDEAL, XSTK(ST_TAB, 8), 0, 4,
   XDEAL, XSTK(ST_TAB, 9), 0, 4,
   XDEAL, XSTK(ST_TAB,10), 0, 4,
   XDEAL, XSTK(ST_TAB,11), 0, 4,
   XDEAL, XSTK(ST_TAB,12), 0, 4,
   XEND
};

aCode validJoin_Fourteen [] = {
   XSTYPE, XSTK1, EQ, ST_DISCARD,
   XSCOUNT, XSTK2, EQ, 2,
   XSSUM, XSTK2, EQ, 14,
      XRETURN, 9,
   XSTYPE, XSTK1, EQ, ST_HAND,
   XSCOUNT, XSTK1, LE, 1,
   XSCOUNT, XSTK2, EQ, 1,
      XRETURN, 5,
   XEND
};

aCode validSplit_Fourteen [] = {
   XSTYPE, XSTK1, EQ, ST_TAB,
   XCOMPARE, XNUM2, EQ, 1,
      XRETURN, 1,
   XSTYPE, XSTK1, EQ, ST_HAND,
      XRETURN, 1,
   XEND
};

/////////////////////////////////////////////////////////////////////////////
// Golf
/////////////////////////////////////////////////////////////////////////////

CGameParams gameParams_Golf [] = {
   "Golf",
   52,
   ST_NUL,
   9999, 0,            // rounds, deals and redeals
};

CCardLayout cardLayout_Golf [] = {
//	      num    offset     based loc    max size     next loc 
   ST_TAB,     5,  {  0,15 },  {  0, 0 },  {   0,  0 },  { 10, 0 },
   ST_DECK,    1,  {  0, 0 },  {  0,30 },  {   0,  0 },  {  0, 0 },
   ST_DISCARD, 1,  {  0, 0 },  { 40,30 },  {   0,  0 },  {  0, 0 },
   ST_NUL,     0,  {  0, 0 },  {  0, 0 },  {   0,  0 },  {  0, 0 },
};

aCode dealCards_Golf [] = {
   // first deal 5 columns of 7 cards face up
   XCOMPARE, XNUM1, EQ, 1,
      XDEAL, XSTK(ST_TAB, 1), 0, 7,
      XDEAL, XSTK(ST_TAB, 2), 0, 7,
      XDEAL, XSTK(ST_TAB, 3), 0, 7,
      XDEAL, XSTK(ST_TAB, 4), 0, 7,
      XDEAL, XSTK(ST_TAB, 5), 0, 7,
   // each hit deals one to discard pile
   XCOMPARE, XNUM1, EQ, 2,
      XDEAL, XSTK(ST_DISCARD, 1), 0, 1,
   XEND
};

aCode validJoin_Golf [] = {
   // drop on discard if it is in sequence (higher or lower) but not King
   XSTYPE, XSTK1, EQ, ST_DISCARD,
   XSHEAD, XSTK1, RANK_NE, RK_KING,
   XSJOIN, XSTK1, SQ_ASCENDING, XSTK2,
      XRETURN, 5,
   XSTYPE, XSTK1, EQ, ST_DISCARD,
   XSJOIN, XSTK1, SQ_DESCENDING, XSTK2,
      XRETURN, 5,
   XEND
};

aCode validSplit_Golf [] = {
   // take any single card from Tableau
   XSTYPE, XSTK1, EQ, ST_TAB,
   XCOMPARE, XNUM2, EQ, 1,
      XRETURN, 1,
   XEND
};

/////////////////////////////////////////////////////////////////////////////
// PussInCorner
/////////////////////////////////////////////////////////////////////////////

CGameParams gameParams_PussInCorner [] = {
   "Puss in Corner",
   52,
   ST_NUL,
   9999, 0,         // rounds, deals and redeals
};

CCardLayout cardLayout_PussInCorner [] = {
//	      num    offset     based loc    max size     next loc 
   ST_FND,     4,  {  0, 0 },  { 10,10 },  {   0,  0 },  { 10, 0 },
   ST_TAB,     2,  {  0,12 },  {  0, 0 },  {  10, 30 },  { 50, 0 },
   ST_TAB,     2,  {  0,12 },  {  0,20 },  {  10, 30 },  { 50, 0 },
   ST_DECK,    1,  {  0, 0 },  { 20,30 },  {   0,  0 },  {  0, 0 },
   ST_HAND,    1,  {  0, 0 },  { 30,30 },  {   0,  0 },  {  0, 0 },
   ST_NUL,     0,  {  0, 0 },  {  0, 0 },  {   0,  0 },  {  0, 0 },
};

aCode dealCards_PussInCorner [] = {
   XCOMPARE, XNUM1, EQ, 1,
      XDEALC, XSTK(ST_FND,1), 0, RK_ACE,
      XDEALC, XSTK(ST_FND,2), 0, RK_ACE,
      XDEALC, XSTK(ST_FND,3), 0, RK_ACE,
      XDEALC, XSTK(ST_FND,4), 0, RK_ACE,
   XCOMPARE, XNUM1, EQ, 2,
      XDEAL, XSTK(ST_HAND,1), 0, 1,
   XEND
};

aCode validJoin_PussInCorner [] = {
   XSTYPE, XSTK1, EQ, ST_FND,
   XSJOIN, XSTK1, SQ_ASCENDING, XSTK2,
      XRETURN, 5,
   XSTYPE, XSTK1, EQ, ST_TAB,
      XRETURN, 5,
   XEND
};

aCode validSplit_PussInCorner [] = {
   XSTYPE, XSTK1, EQ, ST_TAB,
   XCOMPARE, XNUM2, EQ, 1,
      XRETURN, 1,
   XSTYPE, XSTK1, EQ, ST_HAND,
   XCOMPARE, XNUM2, EQ, 1,
      XRETURN, 1,
   XEND
};

/////////////////////////////////////////////////////////////////////////////
// AuldLangSyne
/////////////////////////////////////////////////////////////////////////////

CGameParams gameParams_AuldLangSyne [] = {
   "Auld Lang Syne",
   52,
   ST_NUL,
   9999, 0,         // rounds, deals and redeals
};

CCardLayout cardLayout_AuldLangSyne [] = {
//	      num    offset     based loc    max size     next loc 
   ST_FND,     4,  {  0, 0 },  {  0, 0 },  {   0,  0 },  { 10, 0 },
   ST_TAB,     4,  {  0,12 },  {  0,16 },  {  10, 30 },  { 10, 0 },
   ST_DECK,    1,  {  0, 0 },  { 45, 8 },  {   0,  0 },  {  0, 0 },
   ST_NUL,     0,  {  0, 0 },  {  0, 0 },  {   0,  0 },  {  0, 0 },
};

aCode dealCards_AuldLangSyne [] = {
   XCOMPARE, XNUM1, EQ, 1,
      XDEALC, XSTK(ST_FND,1), 0, RK_ACE,
      XDEALC, XSTK(ST_FND,2), 0, RK_ACE,
      XDEALC, XSTK(ST_FND,3), 0, RK_ACE,
      XDEALC, XSTK(ST_FND,4), 0, RK_ACE,
      XDEAL, XSTK(ST_TAB,1), 0, 1,
      XDEAL, XSTK(ST_TAB,2), 0, 1,
      XDEAL, XSTK(ST_TAB,3), 0, 1,
      XDEAL, XSTK(ST_TAB,4), 0, 1,
   XCOMPARE, XNUM1, EQ, 2,
      XDEAL, XSTK(ST_TAB,1), 0, 1,
      XDEAL, XSTK(ST_TAB,2), 0, 1,
      XDEAL, XSTK(ST_TAB,3), 0, 1,
      XDEAL, XSTK(ST_TAB,4), 0, 1,
   XEND
};

aCode validJoin_AuldLangSyne [] = {
   XSTYPE, XSTK1, EQ, ST_FND,
   XSJOIN, XSTK1, SQ_ASCENDING, XSTK2,
      XRETURN, 5,
   XEND
};

aCode validSplit_AuldLangSyne [] = {
   XSTYPE, XSTK1, EQ, ST_TAB,
   XCOMPARE, XNUM2, EQ, 1,
      XRETURN, 1,
   XEND
};

/////////////////////////////////////////////////////////////////////////////
// Ultima
/////////////////////////////////////////////////////////////////////////////

CGameParams gameParams_Ultima [] = {
   "Ultima",
   52,
   ST_NUL,
   0, 0,       // rounds, deals and redeals
};

CCardLayout cardLayout_Ultima [] = {
//	      num    offset     based loc    max size     next loc 
   ST_FND,     4,  {  0, 0 },  {  0, 0 },  {   0,  0 },  { 10, 0 },
   ST_TAB,     8,  {  0,12 },  {  0,10 },  {  10, 20 },  { 10, 0 },
   ST_DECK,    1,  {  0, 0 },  { 55, 0 },  {   0,  0 },  {  0, 0 },
   ST_HAND,    4,  {  0, 0 },  { 80, 0 },  {   0,  0 },  {  0,10 },
   ST_NUL,     0,  {  0, 0 },  {  0, 0 },  {   0,  0 },  {  0, 0 },
};

aCode dealCards_Ultima [] = {
   XCOMPARE, XNUM1, EQ, 1,
      XDEAL, XSTK(ST_TAB,1), 0, 7,
      XDEAL, XSTK(ST_TAB,2), 0, 7,
      XDEAL, XSTK(ST_TAB,3), 0, 7,
      XDEAL, XSTK(ST_TAB,4), 0, 7,
      XDEAL, XSTK(ST_TAB,5), 0, 6,
      XDEAL, XSTK(ST_TAB,6), 0, 6,
      XDEAL, XSTK(ST_TAB,7), 0, 6,
      XDEAL, XSTK(ST_TAB,8), 0, 6,
   XEND
};

aCode validJoin_Ultima [] = {
   // fill empty foundation with Ace only
   XSCOUNT, XSTK1, EQ, 0,
   XSTYPE, XSTK1, EQ, ST_FND,
   XSTAIL, XSTK2, RANK_EQ, RK_ACE,
      XRETURN, 9,
   // fill other foundation with same suit, ascending
   XSTYPE, XSTK1, EQ, ST_FND,
   XSJOIN, XSTK1, SQ_SAMESUIT|SQ_ASCENDING, XSTK2,
      XRETURN, 5,
   // move any single card to position in hand
   XSCOUNT, XSTK1, EQ, 0,
   XSTYPE, XSTK1, EQ, ST_HAND,
      XRETURN, 1,
   // move any single card to empty space in tableau
   XSCOUNT, XSTK1, EQ, 0,
   XSTYPE, XSTK1, EQ, ST_TAB,
      XRETURN, 2,
   // build down in tableau by same suit, descending
   XSCOUNT, XSTK1, NE, 0,
   XSTYPE, XSTK1, EQ, ST_TAB,
   XSJOIN, XSTK1, SQ_SAMESUIT|SQ_DESCENDING, XSTK2,
      XRETURN, 3,
   XEND
};

aCode validSplit_Ultima [] = {
   XSTYPE, XSTK1, EQ, ST_TAB,
   XCOMPARE, XNUM2, EQ, 1,
      XRETURN, 1,
   XSTYPE, XSTK1, EQ, ST_HAND,
   XCOMPARE, XNUM2, EQ, 1,
      XRETURN, 1,
   XEND
};

/////////////////////////////////////////////////////////////////////////////
// San Marino?
/////////////////////////////////////////////////////////////////////////////

CGameParams gameParams_SanMarino [] = {
   "San Marino",
   52,
   ST_NUL,
   0, 9999,          // rounds, deals and redeals
};

CCardLayout cardLayout_SanMarino [] = {
//	      num    offset     based loc    max size     next loc 
   ST_TAB,     4,  {  0, 0 },  { 20, 0 },  {   0,  0 },  { 10, 0 },
   ST_TAB,     4,  {  0, 0 },  { 20,10 },  {   0,  0 },  { 10, 0 },
   ST_TAB,     4,  {  0, 0 },  { 20,20 },  {   0,  0 },  { 10, 0 },
   ST_TAB,     4,  {  0, 0 },  { 20,30 },  {   0,  0 },  { 10, 0 },
   ST_DECK,    1,  {  0, 0 },  {  0,15 },  {   0,  0 },  {  0, 0 },
   ST_DISCARD, 1,  {  0, 0 },  { 70,15 },  {   0,  0 },  {  0, 0 },
   ST_NUL,     0,  {  0, 0 },  {  0, 0 },  {   0,  0 },  {  0, 0 },
};

aCode dealCards_SanMarino [] = {
   // first deal 5 columns of 7 cards face up
   XCOMPARE, XNUM1, EQ, 1,
      XDEAL, XSTK(ST_TAB, 1), 0, 1,
      XDEAL, XSTK(ST_TAB, 2), 0, 1,
      XDEAL, XSTK(ST_TAB, 3), 0, 1,
      XDEAL, XSTK(ST_TAB, 4), 0, 1,
      XDEAL, XSTK(ST_TAB, 5), 0, 1,
      XDEAL, XSTK(ST_TAB, 6), 0, 1,
      XDEAL, XSTK(ST_TAB, 7), 0, 1,
      XDEAL, XSTK(ST_TAB, 8), 0, 1,
      XDEAL, XSTK(ST_TAB, 9), 0, 1,
      XDEAL, XSTK(ST_TAB,10), 0, 1,
      XDEAL, XSTK(ST_TAB,11), 0, 1,
      XDEAL, XSTK(ST_TAB,12), 0, 1,
      XDEAL, XSTK(ST_TAB,13), 0, 1,
      XDEAL, XSTK(ST_TAB,14), 0, 1,
      XDEAL, XSTK(ST_TAB,15), 0, 1,
      XDEAL, XSTK(ST_TAB,16), 0, 1,
   // gather and redeal
   XCOMPARE, XNUM1, EQ, 3,
      XMOVES, XSTK(ST_TAB,16), XSTK(ST_DECK,1), ALLCARDS,
      XMOVES, XSTK(ST_TAB,15), XSTK(ST_DECK,1), ALLCARDS,
      XMOVES, XSTK(ST_TAB,14), XSTK(ST_DECK,1), ALLCARDS,
      XMOVES, XSTK(ST_TAB,13), XSTK(ST_DECK,1), ALLCARDS,
      XMOVES, XSTK(ST_TAB,12), XSTK(ST_DECK,1), ALLCARDS,
      XMOVES, XSTK(ST_TAB,11), XSTK(ST_DECK,1), ALLCARDS,
      XMOVES, XSTK(ST_TAB,10), XSTK(ST_DECK,1), ALLCARDS,
      XMOVES, XSTK(ST_TAB, 9), XSTK(ST_DECK,1), ALLCARDS,
      XMOVES, XSTK(ST_TAB, 8), XSTK(ST_DECK,1), ALLCARDS,
      XMOVES, XSTK(ST_TAB, 7), XSTK(ST_DECK,1), ALLCARDS,
      XMOVES, XSTK(ST_TAB, 6), XSTK(ST_DECK,1), ALLCARDS,
      XMOVES, XSTK(ST_TAB, 5), XSTK(ST_DECK,1), ALLCARDS,
      XMOVES, XSTK(ST_TAB, 4), XSTK(ST_DECK,1), ALLCARDS,
      XMOVES, XSTK(ST_TAB, 3), XSTK(ST_DECK,1), ALLCARDS,
      XMOVES, XSTK(ST_TAB, 2), XSTK(ST_DECK,1), ALLCARDS,
      XMOVES, XSTK(ST_TAB, 1), XSTK(ST_DECK,1), ALLCARDS,
      XDEAL, XSTK(ST_TAB, 1), 0, 1,
      XDEAL, XSTK(ST_TAB, 2), 0, 1,
      XDEAL, XSTK(ST_TAB, 3), 0, 1,
      XDEAL, XSTK(ST_TAB, 4), 0, 1,
      XDEAL, XSTK(ST_TAB, 5), 0, 1,
      XDEAL, XSTK(ST_TAB, 6), 0, 1,
      XDEAL, XSTK(ST_TAB, 7), 0, 1,
      XDEAL, XSTK(ST_TAB, 8), 0, 1,
      XDEAL, XSTK(ST_TAB, 9), 0, 1,
      XDEAL, XSTK(ST_TAB,10), 0, 1,
      XDEAL, XSTK(ST_TAB,11), 0, 1,
      XDEAL, XSTK(ST_TAB,12), 0, 1,
      XDEAL, XSTK(ST_TAB,13), 0, 1,
      XDEAL, XSTK(ST_TAB,14), 0, 1,
      XDEAL, XSTK(ST_TAB,15), 0, 1,
      XDEAL, XSTK(ST_TAB,16), 0, 1,
   XCOMPARE, XNUM1, EQ, 9,
      XMOVES, XSTK2, XSTK(ST_DISCARD, 1), 2,
   XEND
};

aCode validJoin_SanMarino [] = {
   // drop on same rank
   XSCOUNT, XSTK1, NE, 0,
   XSTYPE, XSTK1, EQ, ST_TAB,
   XSJOIN, XSTK1, SQ_SAMERANK, XSTK2,
   XSPLACE, XSTK1, PL_SAMEROW, XSTK2,
      XRETURN, 1,
   XSCOUNT, XSTK1, NE, 0,
   XSTYPE, XSTK1, EQ, ST_TAB,
   XSJOIN, XSTK1, SQ_SAMERANK, XSTK2,
   XSPLACE, XSTK1, PL_SAMECOL, XSTK2,
      XRETURN, 2,
   XEND
};

aCode validSplit_SanMarino [] = {
   // take any single card from Tableau
   XSTYPE, XSTK1, EQ, ST_TAB,
      XRETURN, 1,
   XEND
};

// first is the default
CGameRulesInfo CGameRulesInfo::GameRulesInfo [] = {
   gameParams_Tower, cardLayout_Tower, validJoin_Tower, validSplit_Tower, dealCards_Tower,
   gameParams_Ultima, cardLayout_Ultima, validJoin_Ultima, validSplit_Ultima, dealCards_Ultima,
   gameParams_Klondike, cardLayout_Klondike, validJoin_Klondike, validSplit_Klondike, dealCards_Klondike,
   gameParams_Spider, cardLayout_Spider, validJoin_Spider, validSplit_Spider, dealCards_Spider,
   gameParams_Cruel, cardLayout_Cruel, validJoin_Cruel, validSplit_Cruel, dealCards_Cruel,
   gameParams_Addition, cardLayout_Addition, validJoin_Addition, validSplit_Addition, dealCards_Addition,
   gameParams_Golf, cardLayout_Golf, validJoin_Golf, validSplit_Golf, dealCards_Golf,
   gameParams_PussInCorner, cardLayout_PussInCorner, validJoin_PussInCorner, validSplit_PussInCorner, dealCards_PussInCorner,
   gameParams_AuldLangSyne, cardLayout_AuldLangSyne, validJoin_AuldLangSyne, validSplit_AuldLangSyne, dealCards_AuldLangSyne,
   gameParams_SanMarino, cardLayout_SanMarino, validJoin_SanMarino, validSplit_SanMarino, dealCards_SanMarino,
   NULL, NULL, NULL, NULL, NULL
};

// end of UniPatSysCard.cpp
